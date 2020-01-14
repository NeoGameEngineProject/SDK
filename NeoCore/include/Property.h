#ifndef PROPERTY_H
#define PROPERTY_H

#include <type_traits>
#include <cstddef>

#include <Color.h>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

#include <string>
#include <cassert>
#include <memory>
#include <vector>

namespace Neo
{

enum PROPERTY_TYPES
{
	UNKNOWN = 0,
	INTEGER,
	UNSIGNED_INTEGER,
	FLOAT,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	MATRIX4x4,
	COLOR,
	STRING,
	PATH,
	BOOL
};

template<typename T>
PROPERTY_TYPES typeOf()
{
	if(std::is_same<T, int>::value)
		return INTEGER;
	else if(std::is_same<T, unsigned int>::value)
		return UNSIGNED_INTEGER;
	else if(std::is_same<T, float>::value)
		return FLOAT;
	else if(std::is_same<T, Vector2>::value)
		return VECTOR2;
	else if(std::is_same<T, Vector3>::value)
		return VECTOR3;
	else if(std::is_same<T, Vector4>::value)
		return VECTOR4;
	else if(std::is_same<T, Matrix4x4>::value)
		return MATRIX4x4;
	else if(std::is_same<T, Color>::value)
		return COLOR;
	else if(std::is_same<T, std::string>::value)
		return STRING;
	else if(std::is_same<T, bool>::value)
		return BOOL;
	
	return UNKNOWN;
}

class Object;
class IProperty 
{
	std::string m_name;
	size_t m_size = 0;
	PROPERTY_TYPES m_type = UNKNOWN;
	
public:
	IProperty(const char* name, PROPERTY_TYPES type = UNKNOWN, size_t size = 0):
		m_name(name), m_size(size), m_type(type) {}
	
	virtual ~IProperty() = default;
	
	const std::string& getName() const { return m_name; }
	size_t getSize() const { return m_size; }
	PROPERTY_TYPES getType() const { return m_type; }
	
	virtual void* data() = 0;
	virtual const void* cdata() const = 0;
	virtual IProperty* clone() const = 0;
	
	template<typename T>
	T& get()
	{
		return *reinterpret_cast<T*>(data());
	}
	
	template<typename T>
	const T& get() const
	{
		return *reinterpret_cast<const T*>(cdata());
	}
	
	template<typename T>
	void set(const T& value)
	{
		assert((typeOf<T>() == m_type 
			|| typeOf<T>() == VECTOR4 && m_type == COLOR
			|| typeOf<T>() == STRING && m_type == PATH) && "Types don't match!");
		assert(sizeof(T) == m_size && "Assigned type is wrong!");
		
		get<T>() = value;
	}
};

template<typename T>
class Property : public IProperty
{
	T* m_data;

public:
	Property(const char* name, T* data):
		IProperty(name, typeOf<T>(), sizeof(T)), m_data(data)
	{
		static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
	}
	
	Property(const char* name, T* data, PROPERTY_TYPES type):
		IProperty(name, type, sizeof(T)), m_data(data)
	{
		static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
	}

	T* get() { return m_data; }
	T& operator*() { return *m_data; }
	void* data() override { return m_data; }
	const void* cdata() const override { return m_data; }

	IProperty* clone() const override // Can't clone since we are tied to external memory \o/
	{
		assert(nullptr);
		return nullptr;
	}
};

template<typename T>
class StaticProperty : public IProperty
{
	T m_data;

public:
	StaticProperty(const char* name):
		IProperty(name, typeOf<T>(), sizeof(T))
	{
		static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
	}
	
	StaticProperty(const char* name, PROPERTY_TYPES type):
		IProperty(name, type, sizeof(T))
	{
		static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
	}
	
	StaticProperty(const char* name, const T& data):
		IProperty(name, typeOf<T>(), sizeof(T)), m_data(data)
	{
		static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
	}
	
	StaticProperty(const char* name, const T& data, PROPERTY_TYPES type):
		IProperty(name, type, sizeof(T)), m_data(data)
	{
		static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
	}

	T* get() { return &m_data; }
	T& operator*() { return m_data; }
	void* data() override { return &m_data; }
	const void* cdata() const override { return &m_data; }

	IProperty* clone() const override
	{
		return new StaticProperty<T>(getName().c_str(), m_data, getType());
	}
};


#define REGISTER_PROPERTY(field) registerProperty(#field, field)
#define REGISTER_PROPERTY_TYPE(field, type) registerProperty(#field, field, type)

class PropertySystem
{
	std::vector<std::unique_ptr<IProperty>> m_properties;

public:
	PropertySystem(const PropertySystem& ps)
	{
		*this = ps;
	}
	
	PropertySystem() = default;
	PropertySystem(PropertySystem&& ps) = default;
	PropertySystem& operator=(PropertySystem&&) = default;
	PropertySystem& operator=(const PropertySystem& ps)
	{
		m_properties.reserve(ps.m_properties.size());
		for(auto& k : ps.m_properties)
			m_properties.push_back(std::unique_ptr<IProperty>(k->clone()));
		
		return *this;
	}
	
	IProperty* getProperty(const char* name) 
	{
		for(auto& k : m_properties)
			if(k->getName() == name)
				return k.get();
			
		return nullptr;
	}
	
	template<typename T>
	T& get(const char* name) { return getProperty(name)->get<T>(); }
	
	template<typename T>
	void setProperty(const char* name, const T& value)
	{
		get<T>(name) = value;
	}
	
	template<typename T>
	void registerProperty(const char* name, T* data)
	{
		if(getProperty(name))
			return;
		
		m_properties.push_back(std::make_unique<Property<T>>(name, data));
	}

	template<typename T>
	void registerProperty(const char* name, T& data)
	{
		if(getProperty(name))
			return;
		
		m_properties.push_back(std::make_unique<Property<T>>(name, &data));
	}
	
	template<typename T>
	void registerProperty(const char* name, T* data, PROPERTY_TYPES type)
	{
		if(getProperty(name))
			return;
		
		m_properties.push_back(std::make_unique<Property<T>>(name, data, type));
	}
	
	template<typename T>
	T& registerProperty(const char* name)
	{
		if(auto prop = getProperty(name))
			return prop->get<T>();

		auto property = new StaticProperty<T>(name);
		m_properties.push_back(std::unique_ptr<IProperty>(property));
		return *property->get();
	}
	
	template<typename T>
	void registerProperty(const char* name, PROPERTY_TYPES type)
	{
		if(getProperty(name))
			return;

		auto property = new StaticProperty<T>(name, type);
		m_properties.push_back(std::unique_ptr<IProperty>(property));
	}
	
	void registerProperty(IProperty* prop)
	{
		assert(prop != nullptr);
		m_properties.push_back(std::unique_ptr<IProperty>(prop));
	}
	
	std::vector<std::unique_ptr<IProperty>>& getProperties() { return m_properties; }
};

}

#endif
