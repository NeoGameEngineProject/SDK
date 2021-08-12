#include <BinaryScene.h>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <behaviors/SceneLinkBehavior.h>
#include <Log.h>
#include <Level.h>

#include <LevelLoader.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Property.h>

#include <NeoEngineFlatbuffer.h>

using namespace Neo;
using namespace flatbuffers;


static void readBehavior(const FlatBuffer::Behavior* v, Level& l, ObjectHandle parent, const std::string& workingDirectory)
{
	auto behavior = Behavior::create(v->name()->str().c_str());

	if(!behavior)
	{
		LOG_ERROR("Unknown behavior was ignored: " << v->name()->str());
		return;
	}

	for(auto* prop : *v->properties())
	{
		auto propName = prop->name()->str();
		auto type = (Neo::PROPERTY_TYPES) prop->type();

		auto* registeredProp = behavior->getProperty(propName.c_str());

		if(registeredProp == nullptr)
		{
			LOG_ERROR("Unregistered property '" << propName << "' of behavior '" << behavior->getName() << "' was ignored.");
			continue;
		}

		if(registeredProp->getType() != type)
		{
			LOG_ERROR("Property types do not match for '" << propName << "' of behavior '" << behavior->getName() << "'! " << registeredProp->getType() << " vs " << type);
			continue;
		}

		switch(type)
		{
			case PROPERTY_TYPES::BOOL: registeredProp->get<bool>() = *reinterpret_cast<const bool*>(prop->data()->data()); break;
			case PROPERTY_TYPES::INTEGER: registeredProp->get<int>() = *reinterpret_cast<const int*>(prop->data()->data()); break;
			case PROPERTY_TYPES::UNSIGNED_INTEGER: registeredProp->get<unsigned int>() = *reinterpret_cast<const unsigned int*>(prop->data()->data()); break;
			case PROPERTY_TYPES::FLOAT: registeredProp->get<float>() = *reinterpret_cast<const float*>(prop->data()->data()); break;
			case PROPERTY_TYPES::VECTOR3: registeredProp->get<Vector3>() = *reinterpret_cast<const Vector3*>(prop->data()->data()); break;
			case PROPERTY_TYPES::VECTOR2: registeredProp->get<Vector2>() = *reinterpret_cast<const Vector2*>(prop->data()->data()); break;

			case PROPERTY_TYPES::COLOR:
			case PROPERTY_TYPES::VECTOR4: registeredProp->get<Vector4>() = *reinterpret_cast<const Vector4*>(prop->data()->data()); break;

			case PROPERTY_TYPES::MATRIX4x4: registeredProp->get<Matrix4x4>() = *reinterpret_cast<const Matrix4x4*>(prop->data()->data()); break;

			case PROPERTY_TYPES::STRING:
			case PROPERTY_TYPES::PATH: registeredProp->get<std::string>() = std::string(*reinterpret_cast<const char*>(prop->data()->data()), prop->data()->size()); break;
		}
	}

	std::stringstream binData;
	binData.write((const char*) v->data()->data(), v->data()->size());
	behavior->deserializeData(l, binData);

	if(!strcmp("SceneLink", behavior->getName()))
	{
		LevelLoader::load(l, workingDirectory + behavior->getProperty("filename")->get<std::string>().c_str(), parent);

		// Load transform from matrix
		parent->updateFromMatrix();

		// Update children with their relative transforms
		parent->makeSubtreeDirty();
		parent->updateMatrix();
	}
	else
		parent->addBehavior(std::move(behavior));
}

static void readObject(const FlatBuffer::Object* v, Object* parent, Level& level, const std::string& workingDirectory)
{
	auto newObject = level.addObject(v->name()->str().c_str());
	memcpy(newObject->getTransform().entries, v->transform()->entries(), 16*sizeof(float));

	for(auto* b : *v->behaviors())
		readBehavior(b, level, newObject, workingDirectory);

	for(auto* c : *v->children())
		readObject(c, newObject.get(), level, workingDirectory);

	parent->addChild(newObject);
	newObject->setParent(parent->getSelf());
	newObject->updateFromMatrix();
}

bool BinaryScene::load(Level& level, std::istream& file, const std::string& workingDirectory, ObjectHandle root)
{
	std::vector<unsigned char> data;

	if(root.empty())
		root = level.getRoot();

	file.seekg(0,std::ios::end);
	data.resize(file.tellg());

	if(data.size() == 0)
	{
		LOG_ERROR("Level file is empty!");
		return false;
	}

	file.seekg(0,std::ios::beg);
	file.read((char*) data.data(), data.size());

	const FlatBuffer::Level* fbLevel = flatbuffers::GetRoot<FlatBuffer::Level>(data.data());
	level.setMainCameraName(fbLevel->mainCameraName()->c_str());

	for(auto* o : *fbLevel->objects())
	{
		readObject(o, root.get(), level, workingDirectory);
	}

	return true;
}

static Offset<FlatBuffer::Property> createProperty(flatbuffers::FlatBufferBuilder& fbb, IProperty* property)
{
	unsigned int size = 0;
	Offset<Vector<uint8_t>> fdata;

	if(property->getType() == STRING || property->getType() == PATH)
	{
		size = property->get<std::string>().size();
		fdata = fbb.CreateVector<uint8_t>((const uint8_t*) property->get<std::string>().c_str(), size);
	}
	else
	{
		size = property->getSize();
		fdata = fbb.CreateVector<uint8_t>((const uint8_t*) property->data(), size);
	}

	auto fname = fbb.CreateString(property->getName());

	FlatBuffer::PropertyBuilder pb(fbb);
	pb.add_type((FlatBuffer::PROPERTY_TYPES) property->getType());
	pb.add_name(fname);
	pb.add_data(fdata);
	pb.add_size(size);
	return pb.Finish();
}

static Offset<FlatBuffer::Behavior> createBehavior(flatbuffers::FlatBufferBuilder& fbb, Behavior* behavior)
{
	Offset<Vector<Offset<FlatBuffer::Property>>> fproperties;

	std::vector<flatbuffers::Offset<FlatBuffer::Property>> properties;
	properties.reserve(behavior->getProperties().size());

	for(auto& c : behavior->getProperties())
	{
		properties.push_back(createProperty(fbb, c));
	}

	fproperties = fbb.CreateVector(properties);
	auto fname = fbb.CreateString(behavior->getName());

	std::stringstream data;
	behavior->serializeData(data);
	auto fdata = fbb.CreateVector<uint8_t>((uint8_t*) data.str().data(), data.str().size());

	FlatBuffer::BehaviorBuilder bb(fbb);
	bb.add_properties(fproperties);
	bb.add_name(fname);
	bb.add_data(fdata);
	return bb.Finish();
}

static Offset<FlatBuffer::Object> createObject(FlatBufferBuilder& fbb, Object* object)
{
	Offset<Vector<Offset<FlatBuffer::Behavior>>> fbehaviors;
	{
		std::vector<flatbuffers::Offset<FlatBuffer::Behavior>> behaviors;
		behaviors.reserve(object->getChildren().size());

		if(!object->getBehavior<SceneLinkBehavior>())
			for(auto& c : object->getBehaviors())
			{
				behaviors.push_back(createBehavior(fbb, c.get()));
			}

		fbehaviors = fbb.CreateVector(behaviors);
	}

	Offset<Vector<Offset<FlatBuffer::Object>>> fchildren;
	{
		std::vector<Offset<FlatBuffer::Object>> children;
		children.reserve(object->getChildren().size());

		if(!object->getBehavior<SceneLinkBehavior>())
			for(auto& c : object->getChildren())
			{
				children.push_back(createObject(fbb, c.get()));
			}

		fchildren = fbb.CreateVector(children);
	}

	auto fname = fbb.CreateString(object->getName().str());

	FlatBuffer::ObjectBuilder ob(fbb);
	ob.add_children(fchildren);
	ob.add_behaviors(fbehaviors);
	ob.add_name(fname);

	FlatBuffer::Matrix4x4 transform;
	memcpy((void*) transform.entries(), object->getTransform().entries, 16*sizeof(float));

	ob.add_transform(&transform);

	return ob.Finish();
}

bool BinaryScene::save(Level& level, std::ostream& file, const std::string& workingDirectory, ObjectHandle root)
{
	flatbuffers::FlatBufferBuilder fbb;

	if(root.empty())
		root = level.getRoot();

	// First: Build children of root object
	std::vector<flatbuffers::Offset<FlatBuffer::Object>> children;
	children.reserve(root->getChildren().size());

	for(auto& c : root->getChildren())
	{
		children.push_back(createObject(fbb, c.get()));
	}

	auto childrenOffset = fbb.CreateVector(children);

	auto mainCamName = fbb.CreateString(level.getMainCameraName());

	FlatBuffer::LevelBuilder lb(fbb);
	lb.add_mainCameraName(mainCamName);
	lb.add_objects(childrenOffset);
	auto levelRoot = lb.Finish();

	fbb.Finish(levelRoot);
	file.write((const char*) fbb.GetBufferPointer(), fbb.GetSize());
	file.flush();

	return true;
}

bool BinaryScene::supportsExtension(const std::string& ext)
{
	return ext == "blv";
}

