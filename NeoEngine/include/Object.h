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
	Object* m_parent = nullptr;
	
	bool m_needsUpdate = true;

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
	Vector3 getPosition() const { return m_position; }
	void setPosition(const Vector3& position) { m_position = position; m_needsUpdate = true; }
	
	Quaternion getRotation() const { return m_rotation; }
	void setRotation(const Quaternion& rotation) { m_rotation = rotation; m_needsUpdate = true; }
	
	Vector3 getScale() const { return m_scale; }
	void setScale(const Vector3& scale) { m_scale = scale; m_needsUpdate = true; }
	
	void rotate(const Vector3& axis, float angle)
	{
		m_rotation *= Quaternion(angle, axis);
		m_needsUpdate = true;
	}
	
	void rotate(const Vector3& euler)
	{
		m_rotation *= Quaternion(euler.x, euler.y, euler.z);
		m_needsUpdate = true;
	}
	
	void translate(const Vector3& offset, bool local = false)
	{
		m_position += local ? m_transform.getRotatedVector3(offset) : offset;
		m_needsUpdate = true;
	}
	
	void scale(const Vector3& scale, bool local = false)
	{
		const Vector3 localScale = local ? m_transform.getRotatedVector3(scale) : scale;
		
		m_scale.x *= localScale.x;
		m_scale.y *= localScale.y;
		m_scale.z *= localScale.z;
		
		m_needsUpdate = true;
	}
	
	const char* getName() const { return m_name; }
	void setName(const char* name);
	
	std::vector<Object*>& getChildren() { return m_children; }
	Object* addChild(Object* object);
	Object* find(const char* name);
	
	Object* getParent() const { return m_parent; }
	void setParent(Object* object) { m_parent = object; } 
	
	void updateMatrix()
	{
		m_transform.setRotationAxis(m_rotation.getAngle(), m_rotation.getAxis());
		m_transform.setTranslationPart(m_position);
		m_transform.scale(m_scale);
		
		if(m_parent)
		{
			m_parent->updateMatrix();
			m_transform = m_parent->getTransform() * m_transform;
		}
		
		m_needsUpdate = false;
	}
	
	void updateFromMatrix()
	{
		m_position = m_transform.getTranslationPart();
		m_rotation.setFromAngles(m_transform.getEulerAngles());
		m_scale = m_transform.getScale();
		m_needsUpdate = false;
	}
	
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
	
	void begin(Platform& p, Renderer& r) 
	{ 
		for(auto& k : m_behaviors)
			k->begin(p, r);
	}
	
	void end() 
	{ 
		for(auto& k : m_behaviors)
			k->end();
	}
	
	void update(const Platform& p, float dt)
	{
		for(auto& k : m_behaviors)
			k->update(p, dt);
		
		// Update matrix after updating the object
		updateMatrix();
	}
	
	void draw(Renderer& r) 
	{ 
		for(auto& k : m_behaviors)
			k->draw(r);
	}
};
}

#endif
