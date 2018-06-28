#include "Level.h"
#include <cstring>
#include <iostream>
#include <cassert>

#include <Texture.h>
#include <TextureLoader.h>
#include <LevelLoader.h>

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

void Level::draw(Renderer& r) 
{
	assert(m_currentCamera);
	r.beginFrame(*this, *m_currentCamera);
	for(size_t i = 0; i < m_numObjects; i++)
		m_objects[i].draw(r);
	r.endFrame();
}

Texture* Level::loadTexture(const char* name)
{
	auto texture = m_textures.find(name);
	if(texture == m_textures.end())
	{
		Texture* tex = &m_textures[name];
		TextureLoader::load(*tex, name);
		return tex;
	}
	
	return &texture->second;
}

bool Level::load(const char* path)
{
	return LevelLoader::load(*this, path);
}

void Level::updateVisibility(const CameraBehavior& camera)
{
	
}

// TODO Culling for lights and objects!
void Level::updateVisibility(const CameraBehavior& camera, Array<LightBehavior*>& visibleLights)
{
	size_t lightNum = 0;
	for(size_t i = 0; i < visibleLights.count && i < m_numObjects; i++)
	{
		auto light = m_objects[i].getBehavior<LightBehavior>();
		if(light)
		{
			visibleLights[lightNum] = light;
			lightNum++;
		}
	}
	
	visibleLights[lightNum] = nullptr;
}
