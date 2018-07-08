#include <Object.h>
#include <cassert>
#include <cstring>

#include <iostream>

using namespace Neo;

Behavior* Object::addBehavior(BehaviorRef&& behavior)
{
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
	strncpy(m_name, name, sizeof(m_name));
	m_name[sizeof(m_name)-1] = 0;
}

ObjectHandle Object::addChild(ObjectHandle object)
{
	object->setParent(m_self);
	m_children.push_back(object);
	return object;
}

ObjectHandle Object::find(const char* name)
{
	if(!strcmp(name, getName()))
		return m_self;
	
	for(auto k : m_children)
	{
		if(!strcmp(k->getName(), name))
			return k;
	}
	
	return ObjectHandle();
}
