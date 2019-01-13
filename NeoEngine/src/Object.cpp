#include <Object.h>
#include <cassert>
#include <cstring>

#include <iostream>

using namespace Neo;

Behavior* Object::addBehavior(BehaviorRef&& behavior)
{
	assert(behavior != nullptr && "Tried to register nullptr!");
	assert(getBehavior(behavior->getName()) == nullptr && "A behavior with that name was already registered!");
	
	auto behaviorPtr = behavior.get();
	
	behavior->setParent(this);
	m_behaviorMap[behavior->getName()] = m_behaviors.size();
	m_behaviors.push_back(std::move(behavior));
	
	return behaviorPtr;
}

void Object::removeBehavior(const char* name)
{
	assert(getBehavior(name) != nullptr && "Trying to delete a non existing behavior!");

	m_behaviors.erase(m_behaviors.begin() + m_behaviorMap[name]);
	m_behaviorMap.erase(name);
}

void Object::removeBehavior(const std::string& name)
{
	removeBehavior(name.c_str());
}

Behavior* Object::getBehavior(const char* name) const
{
	auto result = m_behaviorMap.find(name);
	if(result == m_behaviorMap.end())
		return nullptr;
	
	return m_behaviors[result->second].get();
}

Behavior* Object::getBehavior(const std::string& name) const
{
	return getBehavior(name.c_str());
}

void Object::setName(const char* name)
{
	m_name = name;
}

ObjectHandle Object::addChild(ObjectHandle object)
{
	object->setParent(m_self);
	m_children.push_back(object);
	return object;
}

void Object::removeChild(ObjectHandle child)
{
	for(size_t i = 0; i < m_children.size(); i++)
	{
		auto& c = m_children[i];
		if(c == child)
		{
			child->setParent(ObjectHandle());
			m_children.erase(m_children.begin() + i);
			return;
		}
	}
}

ObjectHandle Object::find(const char* name)
{
	if(name == getName())
		return m_self;
	
	for(auto k : m_children)
	{
		if(k->getName() == name)
			return k;
	}
	
	return ObjectHandle();
}

Box3D Object::getTransformedBoundingBox() const
{
	Box3D aabb = getBoundingBox();
	
	const Matrix4x4 M = getTransform();
	
	const Vector3& min = aabb.min;
	const Vector3& max = aabb.max;
		
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
	
	aabb.min = M * points[0];
	aabb.max = M * points[0];
	
	for(unsigned short i = 1; i < 8; i++)
		aabb.addPoint(M * points[i]);
	
	return aabb;
}

Object& Object::operator= (const Object& obj)
{
	if(this != &obj)
	{
		for(auto& behavior : obj.m_behaviors)
		{
			auto newBehavior = behavior->clone();
			newBehavior->setParent(this);
			
			m_behaviors.push_back(std::move(newBehavior));
		}
	
		for(auto& behavior : obj.m_behaviorMap)
		{
			m_behaviorMap[behavior.first] = behavior.second;
		}
		
		m_transform = obj.m_transform;
		m_rotation = obj.m_rotation;
		m_scale = obj.m_scale;
		
		// FIXME Copy name or not?
		// m_name = obj.m_name;
		
		m_active = obj.m_active;
		m_visible = obj.m_visible;
		
		if(obj.m_linkedFile != nullptr)
			m_linkedFile = std::make_unique<FixedString<256>>(*obj.m_linkedFile);
	}
	return *this;
}
