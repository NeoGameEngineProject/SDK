#ifndef NEO_LEVEL_H
#define NEO_LEVEL_H

#include "NeoEngine.h"
#include "Object.h"

namespace Neo 
{

class NEO_ENGINE_EXPORT Level
{
	Object m_root;
public:
	Level(): m_root("ROOT") {}
	Level(Level&& level) : m_root(std::move(level.m_root)) {}
	
	std::vector<Object>& getObjects() { return m_root.getChildren(); }
	Object* addObject(const char* name) { return m_root.addChild(name); }
	Object* find(const char* name) { return m_root.find(name); }
};

}

#endif // NEO_LEVEL_H
