#ifndef NEO_LEVEL_H
#define NEO_LEVEL_H

#include "NeoEngine.h"
#include "Object.h"
#include <Array.h>

#include <behaviors/CameraBehavior.h>

namespace Neo 
{

class NEO_ENGINE_EXPORT Level
{
	size_t m_numObjects = 0;
	Array<Object> m_objects;
	Array<char> m_scratchpad;
	
	CameraBehavior* m_currentCamera = nullptr;
	
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
	
	CameraBehavior* getCurrentCamera() { return m_currentCamera; }
	void setCurrentCamera(CameraBehavior* cam) { m_currentCamera = cam; }
	
	Array<Object>& getObjects() { return m_objects; }
	Object* addObject(const char* name);
	Object* find(const char* name);
	Object* getRoot() { return &m_objects[0]; }
	
	void begin(Platform& p, Renderer& r) 
	{ 
		for(size_t i = 0; i < m_numObjects; i++)
			m_objects[i].begin(p, r);
	}
	
	void end() 
	{ 
		for(size_t i = 0; i < m_numObjects; i++)
			m_objects[i].end();
	}
	
	void update(const Platform& p, float dt)
	{ 
		for(size_t i = 0; i < m_numObjects; i++)
			m_objects[i].update(p, dt);
	}
	
	void draw(Renderer& r);
};

}

#endif // NEO_LEVEL_H
