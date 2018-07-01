#ifndef NEO_LEVEL_H
#define NEO_LEVEL_H

#include "NeoEngine.h"
#include "Object.h"
#include <Array.h>
#include <Texture.h>

#include <behaviors/CameraBehavior.h>
#include <behaviors/LightBehavior.h>

namespace Neo 
{

/**
 * @brief This class represents a loadable level.
 * 
 * All resources needed are managed by this class. When an object
 * of this class is destroyed, all data belonging to it is destroyed as well.
 * Make sure you do not use any objects, textures and behaviors after that!
 */
class NEO_ENGINE_EXPORT Level
{
	size_t m_numObjects = 0;
	Array<Object> m_objects;
	Array<char> m_scratchpad;
	
	CameraBehavior* m_currentCamera = nullptr;
	
	std::unordered_map<std::string, Texture> m_textures;
	
public:
	/**
	 * @brief Constructs a new level.
	 * @param maxObjects The maximum number of objects this Level will hold.
	 * @param scratchpad The maximum size of the scratchpad memory in bytes.
	 */
	Level(size_t maxObjects = 4096, size_t scratchpad = 4096)
	{
		m_scratchpad.alloc(scratchpad);
		m_objects.alloc(maxObjects+1);
		m_objects[0] = std::move(Object("ROOT"));
		m_numObjects++;
	}
	
	Level(Level&& level):
		m_objects(std::move(level.m_objects)), 
		m_scratchpad(std::move(level.m_scratchpad)),
		m_numObjects(level.m_numObjects) 
		{
			level.m_numObjects = 0;
		}
	
	CameraBehavior* getCurrentCamera() { return m_currentCamera; }
	void setCurrentCamera(CameraBehavior* cam) { m_currentCamera = cam; }
	
	/**
	 * @brief Returns the scratchpad and casts it to the right type.
	 * @tparam T The type required.
	 * @return The scratchpad pointer.
	 */
	template<typename T>
	T* getScratchPad() { return (T*) m_scratchpad.data; }
	
	/**
	 * @brief Calculates the scratchpad size.
	 * @return The size in bytes.
	 */
	size_t getScratchPadSize() const { return m_scratchpad.count; }
	
	/**
	 * @brief Calculates the scratchpad size.
	 * @tparam T The type to calculate the capacity for.
	 * @return The size in multiples of the given type, i.e. the count of objects of the type this buffer can hold.
	 */
	template<typename T>
	size_t getScratchPadSize() const { return m_scratchpad.count / sizeof(T); }

	
	Array<Object>& getObjects() { return m_objects; }
	
	/**
	 * @brief Adds a new object as long as a slot is free.
	 * 
	 * The new object is linked to ROOT by default.
	 * 
	 * @param name The name of the object.
	 * @return The new object.
	 */
	Object* addObject(const char* name);
	
	/**
	 * @brief Finds an object by name.
	 * @param name The name.
	 * @return The object or nullptr.
	 */
	Object* find(const char* name);
	Object* getRoot() { return &m_objects[0]; }
	
	// Assets
	
	/**
	 * @brief Loads a new texture or returns a cached version.
	 * 
	 * If the texture was not loaded yet it will be added to the cache.
	 * If the texture exists the loaded version will be returned.
	 * Textures only live as long as the level lives!
	 * 
	 * @param name The path to the image file.
	 * @return The texture pointer.
	 */
	Texture* loadTexture(const char* name);
	
	/**
	 * @brief Loads a level from a file.
	 * @param path The level file path.
	 * @return \b true on success \b false otherwise.
	 */
	bool load(const char* path);
	
	/**
	 * @brief Updates the visibility of objects relative to the given camera.
	 * @param camera The camera to use as the viewpoint.
	 */
	void updateVisibility(const CameraBehavior& camera);
	
	/**
	 * @brief Updates the visibility of objects relative to the given camera.
	 * 
	 * The given buffer will be filled with all lights that are visible to the camera.
	 * The buffer is sure to remain the same size!
	 * 
	 * @param camera The camera to use as the viewpoint.
	 * @param visibleLights [out] A buffer all lights will be added to.
	 */
	void updateVisibility(const CameraBehavior& camera, Array<LightBehavior*>& visibleLights);
	
	/**
	 * @brief Start level.
	 * 
	 * Initializes all objects and behaviors.
	 * 
	 * @param p The platform context.
	 * @param r The rendering context.
	 */
	void begin(Platform& p, Renderer& r) 
	{ 
		for(size_t i = 0; i < m_numObjects; i++)
			m_objects[i].begin(p, r);
	}
	
	/**
	 * @brief End level.
	 * 
	 * Clears all objects and behaviors.
	 */
	void end() 
	{ 
		for(size_t i = 0; i < m_numObjects; i++)
			m_objects[i].end();
	}
	
	/**
	 * @brief Update level.
	 * 
	 * Updates all objects and behaviors.
	 * 
	 * @param p The platform context.
	 * @param dt The delta time since the last frame.
	 */
	void update(const Platform& p, float dt)
	{
		for(size_t i = 0; i < m_numObjects; i++)
			m_objects[i].update(p, dt);
	}
	
	/**
	 * @brief Draw level.
	 * 
	 * Draws all objects and behaviors.
	 * 
	 * @param r The rendering context.
	 */
	void draw(Renderer& r);
};

}

#endif // NEO_LEVEL_H
