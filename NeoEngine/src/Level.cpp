#include "Level.h"
#include <cstring>
#include <iostream>
#include <cassert>

using namespace Neo;

Object* Level::addObject(const char* name)
{
	assert(m_numObjects < m_objects.count - 1);
	
	auto object = m_objects.data + m_numObjects;
	*object = std::move(Object(name));
	m_numObjects++;
	
	return object;
}

Object* Level::find(const char* name)
{
	for(size_t i = 0; i < m_objects.count; i++)
		if(!strcmp(m_objects[i].getName(), name))
			return &m_objects[i];
		
	return nullptr;
}
