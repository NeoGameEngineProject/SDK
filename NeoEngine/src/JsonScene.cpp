#include <JsonScene.h>

#define RAPIDJSON_ASSERT(x) if(!(x)) throw std::runtime_error("RapidJSON: Assertion failed: " # x)

#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <behaviors/SceneLinkBehavior.h>
#include <Log.h>
#include <Level.h>

#include <LevelLoader.h>

using namespace Neo;
using namespace rapidjson;

// REGISTER_LEVEL_LOADER(JsonScene)

// TODO Base64 or something more effective!
std::string Neo::encodeData(std::istream& in)
{
	std::stringstream result;
	int c;
	while((c = in.get()) != -1)
	{
		result << std::hex << std::setfill('0') << std::setw(2) << (unsigned int) c << " ";
	}

	return result.str();
}

void Neo::decodeData(std::stringstream& out, const std::string& str)
{
	if(str.empty())
		return;
	
	/*
	for(size_t i = 0; i < str.size(); i += 2)
	{
		unsigned int c = std::stoul(str.substr(i, 1), 0, 16);
		out.write((char*) &c, sizeof(c));
	}*/

	std::stringstream in(str);
	while(!!in)
	{
		unsigned int i;
		unsigned char c;
		in >> std::hex >> i;

		c = i;
		out.write((char*) &c, sizeof(c));
	}
}

static void readVector2(const Value& v, Vector2& vec)
{
	vec.x = v[0].GetDouble();
	vec.y = v[1].GetDouble();
}

static void readVector3(const Value& v, Vector3& vec)
{
	vec.x = v[0].GetDouble();
	vec.y = v[1].GetDouble();
	vec.z = v[2].GetDouble();
}

static void readVector4(const Value& v, Vector4& vec)
{
	vec.x = v[0].GetDouble();
	vec.y = v[1].GetDouble();
	vec.z = v[2].GetDouble();
	vec.w = v[3].GetDouble();
}

static void readMatrix(const Value& v, Matrix4x4& mtx)
{
	auto values = v.GetArray();
	for(int i = 0; i < values.Size(); i++)
		mtx.entries[i] = values[i].GetDouble(); // Double because we can only write Doubles.
}

static void readBehavior(const Value& v, Level& l, ObjectHandle parent, const std::string& workingDirectory)
{
	auto behavior = Behavior::create(v["name"].GetString());

	if(!behavior)
	{
		LOG_ERROR("Unknown behavior was ignored: " << v["name"].GetString());
		return;
	}

	auto properties = v["properties"].GetArray();
	for(int i = 0; i < properties.Size(); i++)
	{
		auto& prop = properties[i];
		auto propName = prop["name"].GetString();
		auto type = prop["type"].GetUint();

		auto* registeredProp = behavior->getProperty(propName);

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
			case PROPERTY_TYPES::BOOL: registeredProp->get<bool>() = prop["value"].GetBool(); break;
			case PROPERTY_TYPES::INTEGER: registeredProp->get<int>() = prop["value"].GetInt(); break;
			case PROPERTY_TYPES::UNSIGNED_INTEGER: registeredProp->get<unsigned int>() = prop["value"].GetUint(); break;
			case PROPERTY_TYPES::FLOAT: registeredProp->get<float>() = prop["value"].GetDouble(); break;
			case PROPERTY_TYPES::VECTOR3: readVector3(prop["value"], registeredProp->get<Vector3>()); break;
			case PROPERTY_TYPES::VECTOR2: readVector2(prop["value"], registeredProp->get<Vector2>()); break;

			case PROPERTY_TYPES::COLOR:
			case PROPERTY_TYPES::VECTOR4: readVector4(prop["value"], registeredProp->get<Vector4>()); break;

			case PROPERTY_TYPES::MATRIX4x4: readMatrix(prop["value"], registeredProp->get<Matrix4x4>()); break;

			case PROPERTY_TYPES::STRING:
			case PROPERTY_TYPES::PATH: registeredProp->get<std::string>() = prop["value"].GetString(); break;
		}
	}

	std::stringstream binData;
	decodeData(binData, v["data"].GetString());
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

static void readObject(const Value& v, Object* parent, Level& level, const std::string& workingDirectory)
{
	auto newObject = level.addObject(v["name"].GetString());
	
	readMatrix(v["transform"], newObject->getTransform());

	auto behaviors = v["behaviors"].GetArray();
	for(int i = 0; i < behaviors.Size(); i++)
		readBehavior(behaviors[i], level, newObject, workingDirectory);

	auto children = v["children"].GetArray();
	for(int i = 0; i < children.Size(); i++)
		readObject(children[i], newObject.get(), level, workingDirectory);

	parent->addChild(newObject);
	newObject->setParent(parent->getSelf());
	newObject->updateFromMatrix();
}

bool JsonScene::load(Level& level, std::istream& file, const std::string& workingDirectory, ObjectHandle root)
{
	IStreamWrapper in(file);
	Document doc;
	doc.ParseStream(in);

	if(root.empty())
		root = level.getRoot();

	auto scene = doc["scene"].GetArray();
	for(int i = 0; i < scene.Size(); i++)
		readObject(scene[i], root.get(), level, workingDirectory);

	return true;
}

static void writeVector(PrettyWriter<OStreamWrapper>& out, const Vector2& v)
{
	out.StartArray();
	out.Double(v.x); out.Double(v.y);
	out.EndArray();
}

static void writeVector(PrettyWriter<OStreamWrapper>& out, const Vector3& v)
{
	out.StartArray();
	out.Double(v.x); out.Double(v.y); out.Double(v.z);
	out.EndArray();
}

static void writeVector(PrettyWriter<OStreamWrapper>& out, const Vector4& v)
{
	out.StartArray();
	out.Double(v.x); out.Double(v.y); out.Double(v.z); out.Double(v.w);
	out.EndArray();
}

static void writeMatrix(PrettyWriter<OStreamWrapper>& out, const Matrix4x4& mtx)
{
	out.StartArray();
	for(float v : mtx.entries)
		out.Double(v);
	out.EndArray();
}

static void writeProperty(PrettyWriter<OStreamWrapper>& out, IProperty* property)
{
	out.StartObject();
	
	out.Key("name");
	out.String(property->getName().c_str());
	
	out.Key("type");
	out.Int(property->getType());

	out.Key("value");

	switch(property->getType())
	{
		case PROPERTY_TYPES::BOOL:
			out.Bool(property->get<bool>());
			break;

		case PROPERTY_TYPES::INTEGER:
			out.Int(property->get<int>());
			break;

		case PROPERTY_TYPES::UNSIGNED_INTEGER:
			out.Uint(property->get<unsigned int>());
			break;

		case PROPERTY_TYPES::FLOAT:
			out.Double(property->get<float>());
			break;

		case PROPERTY_TYPES::VECTOR3:
			writeVector(out, property->get<Vector3>());
			break;

		case PROPERTY_TYPES::VECTOR2:
			writeVector(out, property->get<Vector2>());
			break;


		case PROPERTY_TYPES::COLOR:
		case PROPERTY_TYPES::VECTOR4:
			writeVector(out, property->get<Vector4>());
			break;

		case PROPERTY_TYPES::MATRIX4x4:
			writeMatrix(out, property->get<Matrix4x4>());
			break;


		case PROPERTY_TYPES::STRING:
		case PROPERTY_TYPES::PATH:
			out.String(property->get<std::string>().c_str());
			break;

		default:
			out.Null();
	}

	out.EndObject();
}

static void writeBehavior(PrettyWriter<OStreamWrapper>& out, Behavior* behavior)
{
	out.StartObject();

	out.Key("name");
	out.String(behavior->getName());
	
	out.Key("properties");
	out.StartArray();
	for(auto* p : behavior->getProperties())
		writeProperty(out, p);
	out.EndArray();

	std::stringstream ss;
	behavior->serializeData(ss);
	
	out.Key("data");
	out.String(encodeData(ss).c_str());

	out.EndObject();
}

static void writeObject(PrettyWriter<OStreamWrapper>& out, Object* object)
{
	out.StartObject();

	out.Key("name");
	out.String(object->getName().str());

	out.Key("transform");
	writeMatrix(out, object->getTransform());

	out.Key("behaviors");
	out.StartArray();
	for(auto& b : object->getBehaviors())
		writeBehavior(out, b.get());
	out.EndArray();

	out.Key("children");
	out.StartArray();
	
	if(!object->getBehavior<SceneLinkBehavior>())
		for(auto& c : object->getChildren())
			writeObject(out, c.get());

	out.EndArray();
	out.EndObject();
}

bool JsonScene::save(Level& level, std::ostream& file, const std::string& workingDirectory, ObjectHandle root)
{
	OStreamWrapper out(file);
	PrettyWriter<OStreamWrapper> writer(out);

	writer.StartObject();
	writer.Key("scene");
	writer.StartArray();
	
	for(auto& c : level.getRoot()->getChildren())
		writeObject(writer, c.get());

	writer.EndArray();

	writer.EndObject();
	return true;
}

bool JsonScene::supportsExtension(const std::string& ext)
{
	return ext == "jlv";
}

