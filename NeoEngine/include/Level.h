#ifndef NEO_LEVEL_H
#define NEO_LEVEL_H

#include "NeoEngine.h"
#include "Object.h"

#include <Array.h>
#include <Texture.h>
#include <Sound.h>
#include <Mesh.h>
#include <Octree.h>
#include <Box3D.h>

#include <PhysicsContext.h>

#include <behaviors/CameraBehavior.h>
#include <behaviors/LightBehavior.h>
#include <behaviors/SoundBehavior.h>

#include <iosfwd>

#ifndef NEO_SINGLE_THREAD
#include <ThreadPool.h>
#endif

#include <mutex>

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
	static const unsigned int OCTREE_NODE_SIZE = 64;
	
	typedef Octree<ObjectHandle, OCTREE_NODE_SIZE> LevelOctree;
	LevelOctree m_octree;
	bool m_enableCulling = true;

	std::vector<Object> m_objects;
	
	CameraBehavior* m_currentCamera = nullptr;
	std::unordered_map<std::string, Texture> m_textures;
	std::vector<Sound> m_sounds;
	std::vector<Mesh> m_meshes;

	PhysicsContext m_physics;
	FixedString<64> m_mainCameraName;

	std::mutex m_octreeMutex;
	
public:
	/**
	 * @brief Constructs a new level.
	 * @param maxObjects The maximum number of objects this Level will hold.
	 */
	Level(size_t maxObjects = 4096):
		m_octree(Vector3(0, 0, 0), Vector3(4096.0f))
	{
		m_objects.reserve(maxObjects + 1);
		m_objects.push_back(std::move(Object("ROOT")));
	}

	Level(Level&& level):
		m_octree(Vector3(0, 0, 0), Vector3(4096.0f)),
		m_objects(std::move(level.m_objects)), 
		m_enableCulling(level.m_enableCulling),
		m_currentCamera(level.m_currentCamera),
		m_textures(std::move(level.m_textures)),
		m_sounds(std::move(level.m_sounds)),
		m_meshes(std::move(level.m_meshes)),
		m_physics(std::move(level.m_physics))
	{ 
		rebuildOctree();
	}
	
	~Level() {}

	CameraBehavior* getCurrentCamera() { return m_currentCamera; }
	void setCurrentCamera(CameraBehavior* cam) { m_currentCamera = cam; }
	
	void setEnableCulling(bool v) { m_enableCulling = v; }
	bool isCullingEnabled() const { return m_enableCulling; }

	const char* getMainCameraName() const { return m_mainCameraName.str(); }
	void setMainCameraName(const char* p) { m_mainCameraName = p; }

	/**
	 * @brief Makes the given name unique in this level.
	 * @param name [inout] The name to make unique.
	 */
	void makeNameUnique(std::string& name);
	
	/**
	 * @brief Creates a unique name based on the given string.
	 * @param name The base for the name.
	 * @return A new unique name.
	 */
	std::string getUniqueName(const std::string& name);
	std::vector<Object>& getObjects() { return m_objects; }
	
	/**
	 * @brief Adds a new object as long as a slot is free.
	 * 
	 * The new object is linked to ROOT by default.
	 * 
	 * @param name The name of the object.
	 * @return The new object.
	 */
	ObjectHandle addObject(const char* name);
	
	/**
	 * @brief Clones the given object and adds it to the scene.
	 * 
	 * @attention The returned object will not be fully initialized. Call begin to do that.
	 * 
	 * @param name The name of the new object.
	 * @param object THe object to clone.
	 * @return The handle to the new object.
	 */
	ObjectHandle instantiate(const char* name, const Object& object);
	ObjectHandle instantiate(const char* name, ObjectHandle& object) { return instantiate(name, *object); }

	/**
	 * @brief Finds an object by name.
	 * @param name The name.
	 * @return The object or nullptr.
	 */
	ObjectHandle find(const char* name);
	ObjectHandle findInactive(size_t idx = 0);
	
	ObjectHandle getRoot() { return ObjectHandle(&m_objects, 0); }

	PhysicsContext& getPhysicsContext() { return m_physics; }
	
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
	Texture* loadTexture(const char* name, Texture&& origin);

	SoundHandle loadSound(const char* name);
	
	MeshHandle loadMesh(const char* name);
	
	// TODO loadLevel
	MeshHandle addMesh(Mesh&& ref);
	MeshHandle addMesh(const Mesh& ref);
	
	/**
	 * @brief Loads a level from a file.
	 * @param path The level file path.
	 * @return \b true on success \b false otherwise.
	 */
	bool load(const char* path, const char* parentNode = nullptr);
	
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
	void begin(Platform& p, Renderer& r);
	
	/**
	 * @brief End level.
	 * 
	 * Clears all objects and behaviors.
	 */
	void end() 
	{
		for(size_t i = 0; i < m_objects.size(); i++)
			m_objects[i].end();

		m_physics.end();
	}
	
	/**
	 * @brief Update level.
	 * 
	 * Updates all objects and behaviors.
	 * 
	 * @param p The platform context.
	 * @param dt The delta time since the last frame.
	 */
	void update(Platform& p, float dt);
	
	/**
	 * @brief Draw level.
	 * 
	 * Draws all objects and behaviors.
	 * 
	 * @param r The rendering context.
	 */
	void draw(Renderer& r, bool clear = false);
	void draw(Renderer& r, CameraBehavior& camera, bool clear = false);
	
	/**
	 * @brief Casts a ray into the level.
	 * 
	 * It finds a hit point and the object being hit and stores it into the given pointers.
	 * 
	 * @param origin The point of origin.
	 * @param direction The direction to cast into.
	 * @param distance The maximum distance to check for intersections (optional).
	 * @param hitPoint [out] Will be assigned the hit point if set (optional).
	 * @param object [out] Will be assigned the object that was hit if set (optional).
	 * @return \b true if something was hit, \b false otherwise
	 */
	bool castRay(const Vector3& origin, const Vector3& direction, float distance = 1000.0f, Vector3* hitPoint = nullptr, ObjectHandle* hitObject = nullptr);
	void rebuildOctree();

	bool saveBinary(const char* file, ObjectHandle root);
	bool saveBinary(const char* file) { return saveBinary(file, getRoot()); }
	bool loadBinary(const char* file, ObjectHandle insertionPoint = ObjectHandle());
	
	bool serialize(std::ostream& out, ObjectHandle root);
	bool serialize(std::ostream& out) { return serialize(out, getRoot()); }
	bool deserialize(std::istream& in, ObjectHandle insertionPoint = ObjectHandle());
};

}

#endif // NEO_LEVEL_H
