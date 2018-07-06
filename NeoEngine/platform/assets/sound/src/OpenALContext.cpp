#include "OpenALContext.h"

#include <cassert>
#include <iostream>

using namespace Neo;

ALenum convertFormat(SOUND_FORMAT format)
{
	switch(format)
	{
		case SOUND_FORMAT_MONO8 : return AL_FORMAT_MONO8;
		case SOUND_FORMAT_STEREO8: return AL_FORMAT_STEREO8;
		case SOUND_FORMAT_MONO16: return AL_FORMAT_MONO16;
		case SOUND_FORMAT_STEREO16: return AL_FORMAT_STEREO16;
	}
	
	return AL_FORMAT_MONO8;
}

OpenALContext::~OpenALContext()
{
	if(m_device)
		alcCloseDevice(m_device);
}

void OpenALContext::initialize(const char* device)
{
	assert(!m_device && !m_context);
	
	m_device = alcOpenDevice(device);
	if(!m_device)
	{
		std::cerr << "Error: Could not open sound device" << std::endl;
		return;
	}
	
	m_context = alcCreateContext(m_device, nullptr);
	if(!m_context)
	{
		std::cerr << "Error: Could not create OpenAL context" << std::endl;
		alcCloseDevice(m_device);
		return;
	}
	
	alcMakeContextCurrent(m_context);
}

	
ALuint OpenALContext::createBuffer(Sound* sound)
{
	auto b = m_buffers.find(sound->getName());
	if(b == m_buffers.end())
	{
		ALuint buffer = 0;
		alGenBuffers(1, reinterpret_cast<ALuint*>(&buffer));
		CHECK_OPENAL_VAL("Could not create buffer!")
		
		alBufferData(buffer, convertFormat(sound->getFormat()), sound->getData(), sound->getSize(), sound->getSampleRate());
		CHECK_OPENAL_VAL("Could not send buffer data!")
		
		sound->setBufferHandle(buffer);
		return buffer;
	}
	
	return b->second;
}
