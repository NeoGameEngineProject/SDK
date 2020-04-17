#ifndef PROPERTY_H
#define PROPERTY_H

#include <type_traits>
#include <cstddef>

#include <Color.h>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <FixedString.h>

#include <string>
#include <cassert>
#include <memory>
#include <vector>
#include <iostream>

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

template<typename TRAW>
PROPERTY_TYPES typeOf()
{
	using T = typename std::decay<TRAW>::type;

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
	
	int m_userId = 0; ///< Represents a custom user define ID value for example when representing shader Uniforms.

public:
	IProperty(const char* name, PROPERTY_TYPES type = UNKNOWN, size_t size = 0):
		m_name(name), m_size(size), m_type(type) {}
	
	virtual ~IProperty() = default;
	
	const std::string& getName() const { return m_name; }
	size_t getSize() const { return m_size; }
	PROPERTY_TYPES getType() const { return m_type; }
	
	int getUserId() const { return m_userId; }
	void setUserId(int value) { m_userId = value; }

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

	void deserialize(std::istream& in)
	{
		// FIXME: Use FixedString instead of std::string
		Neo::FixedString<128> name;
		name.deserialize(in);
		m_name = name.str();

		in.read((char*) &m_type, sizeof(m_type));
		in.read((char*) &m_size, sizeof(m_size));
	}

	void deserializeData(std::istream& in)
	{
		in.read((char*) cdata(), m_size);
	}

	void serialize(std::ostream& out) const
	{
		FixedString<128> name(m_name);
		name.serialize(out);

		out.write((char*) &m_type, sizeof(m_type));
		out.write((char*) &m_size, sizeof(m_size));

		out.write((char*) cdata(), m_size);
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
		// static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
	}
	
	Property(const char* name, T* data, PROPERTY_TYPES type):
		IProperty(name, type, sizeof(T)), m_data(data)
	{
		// static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
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

static IProperty* createStaticProperty(PROPERTY_TYPES type, const char* name)
{
	switch(type)
	{
		default:
		case UNKNOWN: return nullptr;

		case INTEGER: return new StaticProperty<int>(name);
		case UNSIGNED_INTEGER: return new StaticProperty<unsigned int>(name);
		case FLOAT: return new StaticProperty<float>(name);
		case VECTOR2: return new StaticProperty<Vector2>(name);
		case VECTOR3: return new StaticProperty<Vector3>(name);
		case VECTOR4: return new StaticProperty<Vector4>(name);
		case MATRIX4x4: return nullptr; // return new StaticProperty<Matrix4x4>(name);
		case COLOR: return new StaticProperty<Vector4>(name);
		case STRING: return nullptr; // new StaticProperty<int>();
		case PATH: return nullptr;
		case BOOL: return new StaticProperty<bool>(name);
	}
}

static IProperty* deserializeProperty(std::istream& in)
{
	StaticProperty<char> dummy("");
	dummy.deserialize(in);

	IProperty* prop = createStaticProperty(dummy.getType(), dummy.getName().c_str());
	assert(prop);

	prop->deserializeData(in);
	return prop;
}

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

	void deserialize(std::istream& in)
	{
		uint32_t count;
		in.read((char*) &count, sizeof(count));
		
		for(uint32_t i = 0; i < count; i++)
		{
			m_properties.emplace_back(deserializeProperty(in));
		}
	}

	void serialize(std::ostream& out) const
	{
		uint32_t count = m_properties.size();
		out.write((char*) &count, sizeof(count));

		for(uint32_t i = 0; i < count; i++)
		{
			m_properties[i]->serialize(out);
		}
	}
};

}

inline std::ostream& operator<<(std::ostream& stream, const Neo::IProperty& prop)
{
	prop.serialize(stream);
	return stream;
}

inline std::istream& operator>>(std::istream& stream, Neo::IProperty& prop)
{
	prop.deserialize(stream);
	prop.deserializeData(stream);
	return stream;
}

#endif
