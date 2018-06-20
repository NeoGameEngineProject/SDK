#ifndef _OBJECT_H
#define _OBJECT_H

#include "NeoEngine.h"
#include "Behavior.h"

#include <Vector3.h>
#include <Quaternion.h>
#include <Matrix4x4.h>
#include <vector>
#include <unordered_map>

namespace Neo
{
class NEO_ENGINE_EXPORT Object
{
	Vector3 m_position, m_scale = Vector3(1.0f, 1.0f, 1.0f);
	Quaternion m_rotation;
	
	Matrix4x4 m_transform;
	char m_name[64];
	
	// Contains behaviors for fast iteration
	std::vector<BehaviorRef> m_behaviors;
	
	// Contains a name -> instance mapping for fast access by name
	std::unordered_map<std::string, size_t> m_behaviorMap;
	
	// Children
	// Contains non-owning pointers to children.
	// Caller needs to ensure their lifetime! (Level most of the time)
	std::vector<Object*> m_children;

public:
	Object() : Object("UNNAMMED") {}
	Object(const char* name) { setName(name); }
	Object(Object&& obj)
	{
		*this = std::move(obj);
	}
	
	Behavior* addBehavior(BehaviorRef&& behavior);
	void removeBehavior(const char* name);
	void removeBehavior(const std::string& name);
	
	Behavior* getBehavior(const char* name) const;
	Behavior* getBehavior(const std::string& name) const;
	
	Matrix4x4& getTransform() { return m_transform; }
	const char* getName() const { return m_name; }
	void setName(const char* name);
	
	std::vector<Object*>& getChildren() { return m_children; }
	Object* addChild(Object* object);
	Object* find(const char* name);
	
	template<typename T>
	T* addBehavior()
	{
		auto behavior = std::make_unique<T>();
		auto behaviorPtr = behavior.get();
		
		addBehavior(std::move(behavior));
		return behaviorPtr;
	}
	
	template<typename T>
	T* getBehavior() const
	{
		return reinterpret_cast<T*>(getBehavior(T().getName()));
	}
	
	template<typename T>
	void removeBehavior()
	{
		removeBehavior(T().getName());
	}
	
	Object& operator= (Object&& obj)
	{
		if(this != &obj)
		{
			m_behaviors = std::move(obj.m_behaviors);
			m_behaviorMap = std::move(obj.m_behaviorMap);
			m_children = std::move(obj.m_children);
			m_transform = obj.m_transform;
			m_rotation = obj.m_rotation;
			m_scale = obj.m_scale;
			setName(obj.m_name);
		}
		
		return *this;
	}
};
}

#endif
