#ifndef NEO_OPENALCONTEXT_H
#define NEO_OPENALCONTEXT_H

#include <unordered_map>
#include <SoundContext.h>

#include <al.h>
#include <alc.h>

namespace Neo 
{

class NEO_ENGINE_EXPORT OpenALContext : public SoundContext
{
	ALCdevice* m_device = nullptr;
	ALCcontext* m_context = nullptr;
	
	std::unordered_map<std::string, ALuint> m_buffers;
	
public:
	~OpenALContext();
	void initialize(const char* device) override;
	ALCcontext* getContext() { return m_context; }
	ALuint createBuffer(Sound* sound);
};

#define CHECK_OPENAL(msg) { ALenum error = alGetError(); if(error != AL_NO_ERROR) { std::cerr << "Error: " << msg << std::endl; return; } }
#define CHECK_OPENAL_VAL(msg) { ALenum error = alGetError(); if(error != AL_NO_ERROR) { std::cerr << "Error: " << msg << std::endl; return -1; } }

ALenum neo2al(SOUND_FORMAT format);

}

#endif // NEO_OPENALCONTEXT_H
