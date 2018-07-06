#include "PlatformSoundBehavior.h"
#include <iostream>
#include <Object.h>

using namespace Neo;

PlatformSoundBehavior::~PlatformSoundBehavior()
{
	auto buffer = m_sound->getBufferHandle();
	if(buffer != -1)
	{
		alDeleteBuffers(1, (ALuint*) &buffer);
		CHECK_OPENAL("Could not delete buffer!");
	}
	
	if(m_source != -1)
	{
		alDeleteSources(1, &m_source);
		CHECK_OPENAL("Could not delete source!");
	}
}

void PlatformSoundBehavior::begin(Platform& p, Renderer& render)
{
	initialize(p);
}

void PlatformSoundBehavior::end()
{
	stop();
}

void PlatformSoundBehavior::update(Neo::Platform & p, float dt)
{
	auto parent = getParent();
	alSourcefv(m_source, AL_POSITION, parent->getPosition());
}

void PlatformSoundBehavior::initialize(Platform& p)
{
	auto buffer = m_sound->getBufferHandle();
	if(buffer == -1)
	{
		buffer = reinterpret_cast<OpenALContext*>(&p.getSoundContext())->createBuffer(m_sound.get());
		m_sound->setBufferHandle(buffer);
	}
	
	alGenSources(1, &m_source);
	CHECK_OPENAL("Could not create source!")

	alSourcei(m_source, AL_BUFFER, buffer);
	CHECK_OPENAL("Could not set source buffer!")

	ALfloat sourceVel[] = {0.0, 0.0, 0.0};
	alSourcefv(m_source, AL_VELOCITY, sourceVel);
}

void PlatformSoundBehavior::play()
{
	alSourcePlay(m_source);
	CHECK_OPENAL("Could not play source!")
}

void PlatformSoundBehavior::pause()
{
	alSourcePause(m_source);
	CHECK_OPENAL("Could not pause source!")
}

void PlatformSoundBehavior::stop()
{
	alSourceStop(m_source);
	CHECK_OPENAL("Could not stop source!")
}

float PlatformSoundBehavior::getDuration()
{
	int size;
	int bits;
	int freq;
	
	handle_t buffer = m_sound->getBufferHandle();

	alGetBufferi(buffer, AL_SIZE, &size);
	alGetBufferi(buffer, AL_BITS, &bits);
	alGetBufferi(buffer, AL_FREQUENCY, &freq);

	float duration = size / static_cast<float>((bits/8.0f) * freq);
	return duration;
}

void PlatformSoundBehavior::setLooping(bool v)
{
	alSourcei(m_source, AL_LOOPING, v);
}

bool PlatformSoundBehavior::isLooping()
{
	int value;
	alGetSourcei(m_source, AL_LOOPING, &value);
	return value;
}

void PlatformSoundBehavior::setRelative(bool v)
{
	alSourcei(m_source, AL_SOURCE_RELATIVE, v);
}

bool PlatformSoundBehavior::isRelative()
{
	int value;
	alGetSourcei(m_source, AL_SOURCE_RELATIVE, &value);
	return value;
}

void PlatformSoundBehavior::setRadius(float r)
{
	alSourcef(m_source, AL_REFERENCE_DISTANCE, r);
}

float PlatformSoundBehavior::getRadius()
{
	float value;
	alGetSourcef(m_source, AL_REFERENCE_DISTANCE, &value);
	return value;
}

void PlatformSoundBehavior::setPitch(float r)
{
	alSourcef(m_source, AL_PITCH, r);
}

float PlatformSoundBehavior::getPitch()
{
	float value;
	alGetSourcef(m_source, AL_PITCH, &value);
	return value;
}

void PlatformSoundBehavior::setGain(float r)
{
	alSourcef(m_source, AL_GAIN, r);
}

float PlatformSoundBehavior::getGain()
{
	float value;
	alGetSourcef(m_source, AL_GAIN, &value);
	return value;
}

void PlatformSoundBehavior::setRolloff(float r)
{
	alSourcef(m_source, AL_ROLLOFF_FACTOR, r);
}

float PlatformSoundBehavior::getRolloff()
{
	float value;
	alGetSourcef(m_source, AL_ROLLOFF_FACTOR, &value);
	return value;
}

void PlatformSoundBehavior::setTimeOffset(float r)
{
	alSourcef(m_source, AL_SEC_OFFSET, r);
}

float PlatformSoundBehavior::getTimeOffset()
{
	float value;
	alGetSourcef(m_source, AL_SEC_OFFSET, &value);
	return value;
}

bool PlatformSoundBehavior::isPaused()
{
	int state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	return state == AL_PAUSED;
}

bool PlatformSoundBehavior::isPlaying()
{
	int state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}

float PlatformSoundBehavior::getCurrentTime()
{
	int bits;
	int freq;
	int bOffset;

	ALint buffer;
	alGetSourcei(m_source, AL_BUFFER, &buffer);
	alGetSourcei(m_source, AL_BYTE_OFFSET, &bOffset);

	alGetBufferi(buffer, AL_BITS, &bits);
	alGetBufferi(buffer, AL_FREQUENCY, &freq);

	float timePos = bOffset / static_cast<float>((bits/8.0f) * freq);
	return timePos;
}

