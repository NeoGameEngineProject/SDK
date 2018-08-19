#ifndef _BEHAVIOR_H
#define _BEHAVIOR_H

#include "NeoEngine.h"
#include <Platform.h>
#include <memory>

namespace Neo
{


#if defined(_WIN32)
#if defined(_WIN64)
#define FORCE_UNDEFINED_SYMBOL(x) __pragma(comment (linker, "/export:" #x))
#else
#define FORCE_UNDEFINED_SYMBOL(x) __pragma(comment (linker, "/export:_" #x))
#endif
#else
#define FORCE_UNDEFINED_SYMBOL(x) extern "C" void x(void); void (*__ ## x ## _fp)(void)=&x;
#endif
	
#if defined(WIN32) && (defined(NEO_ENGINE_DLL) || defined(_USRDLL))
#include <windows.h>

#define REGISTER_BEHAVIOR(classname) \
	BOOLEAN WINAPI DllMain(IN HINSTANCE handle, IN DWORD signal, IN LPVOID) { \
	 switch(signal) \
	 { \
		case DLL_PROCESS_ATTACH: \
			Neo::Behavior::registerBehavior(std::unique_ptr<classname>(new classname()));\
		break; \
		case DLL_PROCESS_DETACH:  \
		break; \
	 } \
	 \
	return TRUE; \
}

#elif (defined(__GNUC__) || defined(__clang__))
#include <dlfcn.h>
#define REGISTER_BEHAVIOR(classname) \
	extern "C" __attribute__((externally_visible)) __attribute__((weak)) __attribute__((constructor)) void classname##DllMainConstructor() { \
		Neo::Behavior::registerBehavior(std::unique_ptr<classname>(new classname())); \
	} \
		\
	extern "C"  __attribute__((externally_visible))  __attribute__((destructor)) void classname##DllMainDestructor() { \
		\
	} \
	FORCE_UNDEFINED_SYMBOL(classname##DllMainConstructor); \
	FORCE_UNDEFINED_SYMBOL(classname##DllMainDestructor);

#else
#warning "Using default implementation of runtime registration"
#define REGISTER_BEHAVIOR(classname) namespace { classname g_obj; }
#endif

class Object;
class IProperty 
{
	std::string m_name;
	size_t m_size = 0;
public:
	IProperty(const char* name, size_t size = 0):
		m_name(name), m_size(size) {}
		
	const std::string& getName() const { return m_name; }
	size_t getSize() const { return m_size; }
	virtual void* data() = 0;
};

template<typename T>
class Property : public IProperty
{
	T* m_data;

public:
	Property(const char* name, T& data):
		IProperty(name, sizeof(T)), m_data(&data)
	{
		static_assert(std::is_trivially_copyable<T>::value, "A property must be serializable!");
	}

	T* get() { return m_data; }
	T& operator*() { return *m_data; }
	void* data() override { return m_data; }
};

/**
 * @brief Super class for all behaviors an object might have.
 * 
 * All objects only live as long as the parent lives, make sure
 * you don't use them any longer!
 */
class NEO_ENGINE_EXPORT Behavior
{
	///< Static registry
	static std::vector<std::unique_ptr<Behavior>> s_registry;
	static Behavior* findBehaviorInRegistry(const char* name);

public:
	static std::unique_ptr<Behavior> create(const char* name);
	static void registerBehavior(std::unique_ptr<Behavior>&& behavior);
	
private:
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

	std::vector<IProperty*>& getProperties() { return m_properties; }
	virtual void serialize(std::ostream&) {}
	virtual void deserialize(Level&, std::istream&) {}
};

typedef std::unique_ptr<Behavior> BehaviorRef;
typedef std::shared_ptr<Behavior> BehaviorSharedRef;

}

#endif
