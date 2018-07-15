#include "Level.h"
#include <cstring>
#include <iostream>
#include <cassert>
#include <algorithm>

#include <Texture.h>
#include <TextureLoader.h>
#include <LevelLoader.h>
#include <SoundLoader.h>

using namespace Neo;

ObjectHandle Level::addObject(const char* name)
{
	m_objects.push_back(std::move(Object(name, ObjectHandle(&m_objects, m_objects.size()))));
	return m_objects.back().getSelf();
}

ObjectHandle Level::find(const char* name)
{
	for(size_t i = 0; i < m_objects.size(); i++)
		if(!strcmp(m_objects[i].getName(), name))
			return m_objects[i].getSelf();
		
	return ObjectHandle();
}

void Level::draw(Renderer& r) 
{
	assert(m_currentCamera);
	r.beginFrame(*this, *m_currentCamera);
	for(size_t i = 0; i < m_objects.size(); i++)
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

bool Level::load(const char* path, const char* parentNode)
{
	return LevelLoader::load(*this, path, parentNode);
}

void Level::updateVisibility(const CameraBehavior& camera)
{
	
}

// TODO Culling for lights and objects!
void Level::updateVisibility(const CameraBehavior& camera, Array<LightBehavior*>& visibleLights)
{
	size_t lightNum = 0;
	for(size_t i = 0; i < visibleLights.count && i < m_objects.size(); i++)
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

SoundHandle Level::loadSound(const char* name)
{
	auto soundIter = std::find_if(m_sounds.begin(), m_sounds.end(), [name](const Sound& s) {
		if(!strcmp(s.getName(), name))
			return true;
		
		return false;
	});
	
	if(soundIter != m_sounds.end())
		return SoundHandle(&m_sounds, soundIter - m_sounds.begin());
	
	Sound sound;
	if(!SoundLoader::load(sound, name))
	{
		std::cerr << "Error: Could not load sound " << name << std::endl;
		return SoundHandle();
	}
	
	m_sounds.push_back(std::move(sound));
	return SoundHandle(&m_sounds, m_sounds.size() - 1);
}

