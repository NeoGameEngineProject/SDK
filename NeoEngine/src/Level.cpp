#include "Level.h"
#include <cstring>
#include <iostream>
#include <cassert>
#include <algorithm>

#include <Texture.h>
#include <TextureLoader.h>
#include <LevelLoader.h>
#include <SoundLoader.h>
#include <Property.h>

#include <Maths.h>
#include <behaviors/MeshBehavior.h>
#include <fstream>
#include <Log.h>

using namespace Neo;

void Level::makeNameUnique(std::string& name)
{
	if(find(name.c_str()).empty())
		return;
	
	auto numpos = std::find_if(name.rbegin(), name.rend(), [](char c) {
		return std::isdigit(c);
	});
	
	auto numIdx = std::distance(name.rbegin(), numpos);
	unsigned int id = 1;
	if(numpos != name.rend())
	{
		id = std::stoll(name.substr(numIdx)) + 1;
	}
	
	// Count up until we hit a slot!
	do
	{
		name.erase(numIdx);
		name += std::to_string(id++);
	} while(!find(name.c_str()).empty());
}

std::string Level::getUniqueName(const std::string& name)
{
	std::string retval(name);
	makeNameUnique(retval);
	return retval;
}

ObjectHandle Level::addObject(const char* name)
{
	auto self = ObjectHandle(&m_objects, m_objects.size());
	m_objects.push_back(std::move(Object(name)));
	m_objects.back().setActive(true);
	m_objects.back().setSelf(self);
	
	return self;
}

ObjectHandle Level::find(const char* name)
{
	for(size_t i = 0; i < m_objects.size(); i++)
		if(m_objects[i].getName() == name)
			return m_objects[i].getSelf();
		
	return ObjectHandle();
}

ObjectHandle Level::findInactive(size_t idx)
{
	for(size_t i = idx; i < m_objects.size(); i++)
		if(!m_objects[i].isActive())
			return m_objects[i].getSelf();
	
	return ObjectHandle();
}

void Level::begin(Platform& p, Renderer& r) 
{
	m_physics.begin();
	for(size_t i = 0; i < m_objects.size(); i++)
	{
		m_objects[i].setSelf(ObjectHandle(&m_objects, i));
		m_objects[i].begin(p, r, *this);
	}
	
	rebuildOctree();
	r.compileShaders();
}

void Level::update(Platform& p, float dt)
{
	// First: Update physics
	m_physics.update(dt);

	// Second: Update objects and behaviors!
#ifdef NEO_SINGLE_THREAD
	for(size_t i = 0; i < m_objects.size(); i++)
	{
		auto& o = m_objects[i];
		if(!o.isActive())
			return;
		
		o.update(p, dt);

		if(o.isDirty())
		{
			const auto aabb = o.getBoundingBox();
			const Vector3 oldPos = o.getTransform().getTranslatedVector3(Vector3());
			m_octree.update(oldPos, o.getPosition(), (aabb.max - aabb.min)*0.5f, o.getSelf());
			o.updateMatrix();
		}
	}
#else
	ThreadPool::foreach(m_objects.begin(), m_objects.end(), [&p, dt, this](Object& o){
		
		if(!o.isActive())
			return;
		
		o.update(p, dt);

		if(o.isDirty())
		{
			const auto aabb = o.getTransformedBoundingBox();
			const Vector3 oldPos = o.getTransform().getTranslatedVector3(Vector3());
			o.updateMatrix();

			std::lock_guard<std::mutex> lock(m_octreeMutex);
			m_octree.update(oldPos, o.getPosition(), (aabb.max - aabb.min)*0.5f, o.getSelf());
		}
	});
	
	ThreadPool::synchronize();
#endif
}

void Level::draw(Renderer& r) 
{
	assert(m_currentCamera);
	draw(r, *m_currentCamera);
}

void Level::draw(Renderer& r, CameraBehavior& camera)
{
	r.beginFrame(*this, camera);
	
	const auto frustum = camera.getFrustum();
	const auto sphere = frustum.getSphere();
	const auto box = frustum.getBoundingBox(camera);

	Vector3 origin(sphere.x, sphere.y, sphere.z);

	// Traverse over octree: Only shows relevant objects
	std::lock_guard<std::mutex> lock(m_octreeMutex);
	m_octree.traverse(box.min, box.max, [&r, &frustum](LevelOctree::Position* point) {
		
		// Check for frustum, second try to cull it
		auto& object = std::get<2>(*point);
		const auto aabb = object->getTransformedBoundingBox();
		
		if(!object->isActive())
			return;
		
		if(aabb.getDiameter() != 0)
		{
			Vector3 min = aabb.min;
			Vector3 max = aabb.max;
			
			Vector3 points[8] = {
				Vector3(min.x, min.y, min.z),
				Vector3(min.x, max.y, min.z),
				Vector3(max.x, max.y, min.z),
				Vector3(max.x, min.y, min.z),
				Vector3(min.x, min.y, max.z),
				Vector3(min.x, max.y, max.z),
				Vector3(max.x, max.y, max.z),
				Vector3(max.x, min.y, max.z)
			};
			
			if(frustum.isVolumePointsVisible(points, 8))
				r.draw(object.get()); // object->draw(r);
		}
		else r.draw(object.get()); //object->draw(r);
	});
	
	r.endFrame();
}

Texture* Level::loadTexture(const char* name)
{
	auto texture = m_textures.find(name);
	if(texture == m_textures.end())
	{
		Texture* tex = &m_textures[name];
		TextureLoader::load(*tex, name);
		return tex;
	}
	
	return &texture->second;
}

MeshHandle Level::loadMesh(const char* name)
{
	for(size_t i = 0; i < m_meshes.size(); i++)
	{
		if(!strcmp(m_meshes[i].getName(), name))
		{
			return MeshHandle(&m_meshes, i);
		}
	}
	
	// TODO Load mesh from file!
	LOG_ERROR("Loading meshes from file is not yet supported!");
	exit(-1);
	return MeshHandle();
}

bool Level::load(const char* path, Renderer& render, const char* parentNode)
{
	return LevelLoader::load(*this, path, render, parentNode);
}

void Level::updateVisibility(const CameraBehavior& camera)
{
	
}

void Level::rebuildOctree()
{
	std::lock_guard<std::mutex> lock(m_octreeMutex);
	m_octree.clear();
	for(size_t i = 0; i < m_objects.size(); i++)
	{
		auto& obj = m_objects[i];
		auto* mesh = obj.getBehavior<MeshBehavior>();
		if(!mesh)
		{
			m_octree.insert(obj.getPosition(), Vector3(1, 1, 1), ObjectHandle(&m_objects, i));
		}
		else
		{
			mesh->updateBoundingBox();
			auto aabb = mesh->getBoundingBox();
			obj.setBoundingBox(aabb);

			aabb = obj.getTransformedBoundingBox();
			m_octree.insert(obj.getPosition(), (aabb.max - aabb.min)*0.5f, ObjectHandle(&m_objects, i));
		}
	}
}

// TODO Culling for lights and objects!
void Level::updateVisibility(const CameraBehavior& camera, Array<LightBehavior*>& visibleLights)
{
	size_t lightNum = 0;
	for(size_t i = 0; i < visibleLights.count && i < m_objects.size(); i++)
	{
		auto light = m_objects[i].getBehavior<LightBehavior>();
		if(light)
		{
			visibleLights[lightNum] = light;
			lightNum++;
		}
	}
	visibleLights[lightNum] = nullptr;
}

SoundHandle Level::loadSound(const char* name)
{
	auto soundIter = std::find_if(m_sounds.begin(), m_sounds.end(), [name](const Sound& s) {
		if(!strcmp(s.getName(), name))
			return true;
		
		return false;
	});
	
	if(soundIter != m_sounds.end())
		return SoundHandle(&m_sounds, soundIter - m_sounds.begin());
	
	Sound sound;
	if(!SoundLoader::load(sound, name))
	{
		std::cerr << "Error: Could not load sound " << name << std::endl;
		return SoundHandle();
	}
	
	m_sounds.push_back(std::move(sound));
	return SoundHandle(&m_sounds, m_sounds.size() - 1);
}

MeshHandle Level::addMesh(Mesh&& ref)
{
	auto handle = MeshHandle(&m_meshes, m_meshes.size());
	m_meshes.push_back(std::move(ref));
	return handle;
}

MeshHandle Level::addMesh(const Mesh& ref)
{
	auto handle = MeshHandle(&m_meshes, m_meshes.size());
	m_meshes.push_back(ref);
	return handle;
}

bool Level::castRay(const Vector3& origin, const Vector3& direction, float distance, Vector3* hitPoint, ObjectHandle* hitObject)
{
	Vector3 dest = origin + direction * distance; // TODO: As argument?
	float hitDistance = distance;
	Vector3 point;
	bool hitFound = false;
	
	for(auto& object : m_objects)
	{
		auto mesh = object.getBehavior<MeshBehavior>();
		Matrix4x4 iMatrix = object.getTransform().getInverse();
		Vector3 localDest = iMatrix * dest;
		Vector3 localOrigin = iMatrix * origin;
		
		if(!object.isActive() 
			|| !mesh 
			|| !isEdgeToBoxCollision(localOrigin, localDest,
						 mesh->getBoundingBox().min,
						 mesh->getBoundingBox().max))
			continue;

		for(auto& submesh : mesh->getMeshes())
		{
			auto& indices = submesh->getIndices();
			auto& vertices = submesh->getVertices();
			
			for(size_t i = 0; i < indices.size();)
			{
				const Vector3& a = vertices[indices[i++]].position;
				const Vector3& b = vertices[indices[i++]].position;
				const Vector3& c = vertices[indices[i++]].position;
				const Vector3 normal = getTriangleNormal(a, b, c);

				if(!isEdgeTriangleIntersection(localOrigin, localDest, a, b, c, normal, &point))
					continue;
				
				point = object.getTransform() * point;
				float distance = (point - origin).getLength();
				if(distance < hitDistance)
				{
					hitDistance = distance;
					if(hitPoint) *hitPoint = point;
					if(hitObject) *hitObject = object.getSelf();
					
					hitFound = true;
					continue;
				}
			}
		}
	}
	
	return hitFound;
}

ObjectHandle Level::instantiate(const char* name, const Object& object)
{
	auto newObject = addObject(name);
	*newObject = object;
	return newObject;
}

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#define LEVEL_VERSION 1
#define LEVEL_MAGIC 0xDEADBEEF

typedef float float32_t;
typedef double float64_t;

PACK(struct Header
{
	uint8_t version = LEVEL_VERSION;
	uint32_t magic = LEVEL_MAGIC;
	uint32_t objectCount = 0;
});

PACK(struct BinObject
{
	float32_t transform[16];
	uint8_t active;
	uint16_t behaviorCount;
	
	// name FixedString
	// parent FixedString
});

PACK(struct BinProperty
{
	 uint16_t size;
});

namespace
{

bool saveObject(std::ostream& out, ObjectHandle object)
{
	if(object.empty() || !object->isActive())
		return true;

	object->getName().serialize(out);
	
	BinObject obj;
	memcpy(obj.transform, object->getTransform().entries, sizeof(obj.transform));
	obj.active = object->isActive();
	obj.behaviorCount = object->getBehaviors().size();

	out.write((char*) &obj, sizeof(BinObject));

	// Write file link or an empty string to signify if it's a link or an actual object
	if(object->getLinkedFile())
	{
		object->getLinkedFile()->serialize(out);
		return true;
	}
	else
	{
		FixedString<1>().serialize(out);
	}
	
	for(auto& behavior : object->getBehaviors())
	{
		FixedString<64> bname = behavior->getName();
		bname.serialize(out);

		uint16_t value = behavior->getProperties().size();
		out.write((char*) &value, sizeof(value));
		
		for(auto& property : behavior->getProperties())
		{
			BinProperty bp;
			FixedString<64> name = property->getName();
			name.serialize(out);

			bp.size = property->getSize();

			out.write((char*) &bp, sizeof(BinProperty));
			out.write((char*) property->data(), property->getSize());
		}

		behavior->serialize(out);
	}

	uint16_t value = object->getChildren().size();
	out.write((char*) &value, sizeof(value));
	
	bool retval = true;
	for(auto& child : object->getChildren())
	{
		saveObject(out, child);
	}

	return retval;
}


bool loadObject(Level& level, std::istream& in, ObjectHandle parent)
{
	//auto object = level.addObject();
	FixedString<128> name;
	name.deserialize(in);
	
	auto object = level.addObject(name.str());
	
	BinObject obj;
	in.read((char*) &obj, sizeof(obj));
	
	object->getTransform() = obj.transform;
	object->setActive(obj.active);
	object->getBehaviors().reserve(obj.behaviorCount);
	
	// Check if it's a link!
	{
		FixedString<256> link;
		link.deserialize(in);
		
		if(link.getLength())
		{
			object->updateFromMatrix();
			object->setParent(parent);
			parent->addChild(object);
	
			return level.loadBinary(link.str(), object);
		}
	}
	
	for(unsigned short i = 0; i < obj.behaviorCount; i++)
	{
		FixedString<64> bname;
		bname.deserialize(in);
		
		auto behavior = Behavior::create(bname.str());
		if(!behavior)
		{
			return false;
		}
		
		
		uint16_t numProperties;
		in.read((char*) &numProperties, sizeof(numProperties));
		
		for(unsigned short j = 0; j < numProperties; j++)
		{
			FixedString<64> pname;
			pname.deserialize(in);
			
			BinProperty bp;
			in.read((char*) &bp, sizeof(bp));
			
			auto prop = behavior->getProperty(pname.str());
			if(!prop || prop->getSize() != bp.size)
			{
				LOG_WARNING("Invalid property: " << pname << " of behavior " << bname);
				in.ignore(bp.size);
			}
			else
			{
				in.read((char*) prop->data(), bp.size);
			}
		}
		
		behavior->deserialize(level, in);
		object->addBehavior(std::move(behavior));
	}
	
	uint16_t childCount = 0;
	in.read((char*) &childCount, sizeof(childCount));
	object->getChildren().reserve(childCount);
	
	object->updateFromMatrix();
	object->setParent(parent);
	parent->addChild(object);
	
	bool success = true;
	for(uint16_t i = 0; i < childCount; i++)
	{
		success &= loadObject(level, in, object);
	}
	
	return success;
}
}

bool Level::saveBinary(const char* file, ObjectHandle root)
{
	std::ofstream out;
	out.open(file, std::ios::binary);
	if(!out)
	{
		LOG_ERROR("Could not open level file for writing: " << file);
		return false;
	}
	
	if(root != getRoot())
		root->setLinkedFile(std::make_unique<FixedString<256>>(file));

	return serialize(out, root);
}

bool Level::loadBinary(const char* file, ObjectHandle insertionPoint)
{
	std::ifstream in;
	in.open(file, std::ios::binary);
	if(!in)
	{
		LOG_ERROR("Could not open level file for reading: " << file);
		return false;
	}

	return deserialize(in, insertionPoint);
}

bool Level::serialize(std::ostream& out, ObjectHandle root)
{
	// Write header
	Header header;
	header.version = LEVEL_VERSION;
	header.magic = LEVEL_MAGIC;
	header.objectCount = m_objects.size();

	out.write((const char*) &header, sizeof(header));

	// Write meshes
	uint32_t numMeshes = m_meshes.size();
	out.write((char*) &numMeshes, sizeof(numMeshes));
	for(auto& mesh : m_meshes)
		mesh.serialize(out);
	
	return saveObject(out, root);
}

bool Level::deserialize(std::istream& in, ObjectHandle insertionPoint)
{
	// Read header
	Header header;
	in.read((char*) &header, sizeof(header));

	if(header.magic != LEVEL_MAGIC)
	{
		LOG_ERROR("Level file is damaged: Magic is corrupted!");
		return false;
	}
	
	if(header.version > LEVEL_VERSION)
	{
		LOG_WARNING("Level file was written by a newer version of the engine!");
	}
	
	// Read meshes
	uint32_t numMeshes;
	in.read((char*) &numMeshes, sizeof(numMeshes));
	m_meshes.resize(numMeshes);
	for(auto& mesh : m_meshes)
		mesh.deserialize(*this, in);
	
	m_objects.reserve(header.objectCount);
	return loadObject(*this, in, (insertionPoint.empty() ? getRoot() : insertionPoint));
}

