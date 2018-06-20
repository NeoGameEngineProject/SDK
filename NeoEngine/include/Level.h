#ifndef NEO_LEVEL_H
#define NEO_LEVEL_H

#include "NeoEngine.h"
#include "Object.h"
#include <Array.h>

namespace Neo 
{

class NEO_ENGINE_EXPORT Level
{
	size_t m_numObjects = 0;
	Array<Object> m_objects;
	Array<char> m_scratchpad;
	
public:
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
	
	Array<Object>& getObjects() { return m_objects; }
	Object* addObject(const char* name);
	Object* find(const char* name);
	Object* getRoot() { return &m_objects[0]; }
};

}

#endif // NEO_LEVEL_H
