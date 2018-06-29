#ifndef _BEHAVIOR_H
#define _BEHAVIOR_H

#include "NeoEngine.h"
#include <Platform.h>
#include <memory>

namespace Neo
{
class Object;
class NEO_ENGINE_EXPORT Behavior
{
	Object* m_parent = nullptr;
public:
	virtual ~Behavior() {}
	virtual const char* getName() const = 0;
	
	virtual void begin(const Platform& p, Renderer& render) {}
	virtual void update(const Platform& p, float dt) {}
	virtual void draw(Renderer& render) {}
	virtual void end() {}
	
	Object* getParent() { return m_parent; }
	void setParent(Object* parent) { m_parent = parent; }
};

typedef std::unique_ptr<Behavior> BehaviorRef;
typedef std::shared_ptr<Behavior> BehaviorSharedRef;

}

#endif
