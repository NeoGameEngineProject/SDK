#ifndef NEO_PLATFORMSOUNDBEHAVIOR_H
#define NEO_PLATFORMSOUNDBEHAVIOR_H

#include <Behavior.h>
#include <OpenALContext.h>

namespace Neo
{

class PlatformSoundBehavior : public Neo::Behavior
{
	SoundHandle m_sound;
	ALuint m_source = -1;
public:
	PlatformSoundBehavior() {}
	PlatformSoundBehavior(const SoundHandle& sh): m_sound(sh) {}
	~PlatformSoundBehavior();
	
	void begin(Platform& p, Renderer& render) override;
	void end() override;
	
	void update(Neo::Platform & p, float dt) override;
	
	void initialize(Platform& p);
	
	void play();
	void pause();
	void stop();
	
	SoundHandle getSound() { return m_sound; }
	
	void setLooping(bool v);
	bool isLooping();
	
	void setRelative(bool v);
	bool isRelative();
	
	void setRadius(float r);
	float getRadius();
	
	void setPitch(float r);
	float getPitch();
	
	void setGain(float r);
	float getGain();
	
	void setRolloff(float r);
	float getRolloff();
	
	void setTimeOffset(float r);
	float getTimeOffset();
	
	bool isPaused();
	bool isPlaying();
	
	float getCurrentTime();
	float getDuration();
};

}

#endif // NEO_PLATFORMSOUNDBEHAVIOR_H
