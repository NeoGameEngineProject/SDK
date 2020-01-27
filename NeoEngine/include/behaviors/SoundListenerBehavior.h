#ifndef NEO_SOUNDLISTENERBEHAVIOR_H
#define NEO_SOUNDLISTENERBEHAVIOR_H

#include <Sound.h>
#include <SoundContext.h>
#include <PlatformSoundListenerBehavior.h>

namespace Neo 
{
class NEO_ENGINE_EXPORT SoundListenerBehavior : public Neo::PlatformSoundListenerBehavior
{
public:
	SoundListenerBehavior() {}
	
	void copyTo(Behavior& b) const override {}
	const char* getName() const override { return "SoundListener"; }
	Behavior* getNew() const override { return new SoundListenerBehavior; }
};

}

#endif
