#ifndef _OBJECT_H
#define _OBJECT_H

#include "NeoEngine.h"
#include "Behavior.h"

#include <Handle.h>
#include <Vector3.h>
#include <Quaternion.h>
#include <Matrix4x4.h>
#include <FixedString.h>

#include <vector>
#include <unordered_map>

namespace Neo
{
class Object;
typedef Handle<Object, std::vector<Object>> ObjectHandle;
typedef Handle<Object, Array<Object>> ObjectHandleArray;

class NEO_ENGINE_EXPORT Object
{
private:
	Vector3 m_position, m_scale = Vector3(1.0f, 1.0f, 1.0f);
	Quaternion m_rotation;
	
	Matrix4x4 m_transform;
	FixedString<64> m_name;
	
	// Contains behaviors for fast iteration
	std::vector<BehaviorRef> m_behaviors;
	
	// Contains a name -> instance mapping for fast access by name
	std::unordered_map<std::string, size_t> m_behaviorMap;
	
	// Children
	///< Contains non-owning pointers to children.
	///< Caller needs to ensure their lifetime! (Level most of the time)
	std::vector<ObjectHandle> m_children;
	ObjectHandle m_parent;
	
	bool m_needsUpdate = true;
	bool m_visible = true;
	bool m_active = true;
	
	ObjectHandle m_self;
	
	///< The path of the file this object is referring to e.g. when merging scenes
	std::unique_ptr<FixedString<256>> m_linkedFile;

public:
	Object(ObjectHandle self = ObjectHandle()) : Object("UNNAMED", self) {}
	Object(const char* name, ObjectHandle self = ObjectHandle()): m_self(self) { setName(name); }
	Object(Object&& obj)
	{
		*this = std::move(obj);
	}
	
	ObjectHandle getSelf() { return m_self; }
	
	/**
	 * @brief Registers a new behavior.
	 * 
	 * The behavior is bound to this object and will be destroyed when
	 * the object is destroyed.
	 * 
	 * @param behavior The behavior.
	 * @return The behavior pointer.
	 */
	Behavior* addBehavior(BehaviorRef&& behavior);
	
	/**
	 * @brief Removes a behavior.
	 * @param name  The name of the behavior to remove.
	 */
	void removeBehavior(const char* name);
	
	/**
	 * @brief Removes a behavior.
	 * @param name  The name of the behavior to remove.
	 */
	void removeBehavior(const std::string& name);
	
	/**
	 * @brief Finds a behavior.
	 * @param name  The name of the behavior to find.
	 * @return The behavior or \b nullptr
	 */
	Behavior* getBehavior(const char* name) const;
	
	/**
	 * @brief Finds a behavior.
	 * @param name  The name of the behavior to find.
	 * @return The behavior or \b nullptr
	 */
	Behavior* getBehavior(const std::string& name) const;

	std::vector<BehaviorRef>& getBehaviors() { return m_behaviors; }
	const std::vector<BehaviorRef>& getBehaviors() const { return m_behaviors; }

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
	
	const IString& getName() const { return m_name; }
	void setName(const char* name);
	
	std::vector<ObjectHandle>& getChildren() { return m_children; }
	ObjectHandle addChild(ObjectHandle object);
	void removeChild(ObjectHandle child);
	ObjectHandle find(const char* name);
	
	ObjectHandle getParent() const { return m_parent; }
	void setParent(ObjectHandle object) { m_parent = object; } 
	
	void setActive(bool v) { m_active = v; }
	bool isActive() const { return m_active; }
	
	void setVisible(bool v) { m_visible = v; }
	bool isVisible() const { return m_visible; }
	
	FixedString<256>* getLinkedFile() { return m_linkedFile.get(); }
	void setLinkedFile(std::unique_ptr<FixedString<256>>&& str) { m_linkedFile = std::move(str); }
	
	/**
	 * @brief Updates the transformation matrix based on position, rotation and scale.
	 */
	void updateMatrix()
	{
		m_transform.setRotationAxis(m_rotation.getAngle(), m_rotation.getAxis());
		m_transform.setTranslationPart(m_position);
		m_transform.scale(m_scale);
		
		if(!m_parent.empty())
		{
			m_parent->updateMatrix();
			m_transform = m_parent->getTransform() * m_transform;
		}
		
		m_needsUpdate = false;
	}
	
	/**
	 * @brief Updates position, rotation and scale based on the transformation matrix.
	 */
	void updateFromMatrix()
	{
		m_position = m_transform.getTranslationPart();
		m_rotation.setFromAngles(m_transform.getEulerAngles());
		m_scale = m_transform.getScale();
		m_needsUpdate = false;
	}
	
	/**
	 * @brief Adds a new behavior with the given type.
	 * @tparam T The behavior type to add.
	 * @return The new behavior.
	 */
	template<typename T>
	T* addBehavior()
	{
		auto behavior = std::make_unique<T>();
		auto behaviorPtr = behavior.get();
		
		addBehavior(std::move(behavior));
		return behaviorPtr;
	}
	
	/**
	 * @brief Finds the behavior with the given type.
	 * @tparam T The type.
	 * @return The behavior.
	 */
	template<typename T>
	T* getBehavior() const
	{
		return reinterpret_cast<T*>(getBehavior(T().getName()));
	}
	
	/**
	 * @brief Removes the behavior with the given type.
	 * @tparam T The type.
	 */
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
			
			m_name = obj.m_name;
			
			m_active = obj.m_active;
			m_visible = obj.m_visible;
			m_self = obj.m_self;
			
			m_linkedFile = std::move(obj.m_linkedFile);
		}
		
		return *this;
	}
	
	Object& operator= (const Object& obj);
	
	void begin(Platform& p, Renderer& r, Level& level) 
	{ 
		for(auto& k : m_behaviors)
			k->begin(p, r, level);
	}
	
	void end() 
	{ 
		for(auto& k : m_behaviors)
			k->end();
	}
	
	void update(Platform& p, float dt)
	{
		if(!m_active)
			return;
		
		for(auto& k : m_behaviors)
			k->update(p, dt);
		
		// Update matrix after updating the object
		updateMatrix();
	}
	
	void draw(Renderer& r) 
	{
		if(!m_visible || !m_active)
			return;
		
		for(auto& k : m_behaviors)
			k->draw(r);
	}
};
}

#endif
