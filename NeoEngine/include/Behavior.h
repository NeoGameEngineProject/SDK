#ifndef _BEHAVIOR_H
#define _BEHAVIOR_H

#include "NeoEngine.h"
#include <Platform.h>
#include <memory>

namespace Neo
{
class Object;

class IProperty 
{
	std::string m_name;
public:
	IProperty(const char* name):
		m_name(name) {}
		
	const std::string& getName() const { return m_name; }
};

template<typename T>
class Property : public IProperty
{
	T* m_data;

public:
	Property(const char* name, T& data):
		IProperty(name), m_data(&data) {}

	T* get() { return m_data; }
	T& operator*() { return *m_data; }
};

/**
 * @brief Super class for all behaviors an object might have.
 * 
 * All objects only live as long as the parent lives, make sure
 * you don't use them any longer!
 */
class NEO_ENGINE_EXPORT Behavior
{
	///< The parent object.
	Object* m_parent = nullptr;
	std::vector<IProperty*> m_properties;
	
public:
	virtual ~Behavior() 
	{
		for(auto k : m_properties)
			delete k;
	}
	
	/**
	 * @brief Clones the behavior into a new object.
	 */
	virtual std::unique_ptr<Behavior> clone() const;
	
	virtual Behavior* getNew() const = 0;
	
	/**
	 * @brief Returns a static string containing the name of the behavior
	 * @return A static string.
	 */
	virtual const char* getName() const = 0;
	
	/**
	 * @brief Called when the level starts.
	 * 
	 * This initializes all internals of the behavior.
	 * 
	 * @param p The platform context.
	 * @param render The rendering context.
	 */
	virtual void begin(Platform& p, Renderer& render) {}
	
	/**
	 * @brief Updates the behavior.
	 * @param p The platform context.
	 * @param dt The delta time since the last frame.
	 */
	virtual void update(Platform& p, float dt) {}
	
	/**
	 * @brief Draws graphics needed by the behavior.
	 * @param render The rendering context.
	 */
	virtual void draw(Renderer& render) {}
	
	/**
	 * @brief Called when the level ends.
	 * 
	 * This frees up all resources aquired in begin.
	 */
	virtual void end() {}
	
	Object* getParent() { return m_parent; }
	void setParent(Object* parent) { m_parent = parent; }
	
	/**
	 * @brief Returns the property belonging to the given name.
	 * 
	 * @attention Each behavior needs to override this!
	 * @param name The name of the property.
	 * @return The property.
	 */
	IProperty* getProperty(const char* name) 
	{
		for(auto k : m_properties)
			if(k->getName() == name)
				return k;
			
		return nullptr;
	}
	
	template<typename T>
	Property<T>* getProperty(const char* name) { return reinterpret_cast<Property<T>>(getProperty(name)).get(); }
	
	template<typename T>
	T* getProperty(const char* name) { return getProperty<T>(name)->get(); }
	
	template<typename T>
	void setProperty(const char* name, const T& value)
	{
		*getProperty<T>(name) = value;
	}
	
	template<typename T>
	void registerProperty(const char* name, T& data)
	{
		m_properties.push_back(new Property<T>(name, data));
	}
};

typedef std::unique_ptr<Behavior> BehaviorRef;
typedef std::shared_ptr<Behavior> BehaviorSharedRef;

}

#endif
