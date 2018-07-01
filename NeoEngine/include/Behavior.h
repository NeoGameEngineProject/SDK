#ifndef _BEHAVIOR_H
#define _BEHAVIOR_H

#include "NeoEngine.h"
#include <Platform.h>
#include <memory>

namespace Neo
{
class Object;

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
public:
	virtual ~Behavior() {}
	
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
	virtual void begin(const Platform& p, Renderer& render) {}
	
	/**
	 * @brief Updates the behavior.
	 * @param p The platform context.
	 * @param dt The delta time since the last frame.
	 */
	virtual void update(const Platform& p, float dt) {}
	
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
};

typedef std::unique_ptr<Behavior> BehaviorRef;
typedef std::shared_ptr<Behavior> BehaviorSharedRef;

}

#endif
