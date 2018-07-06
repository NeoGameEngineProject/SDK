#ifndef NEO_SOUNDBEHAVIOR_H
#define NEO_SOUNDBEHAVIOR_H

#include <Sound.h>
#include <SoundContext.h>
#include <PlatformSoundBehavior.h>

namespace Neo 
{
class SoundBehavior : public Neo::PlatformSoundBehavior
{
public:
	SoundBehavior() {}
	SoundBehavior(const SoundHandle& sh): PlatformSoundBehavior(sh) {}
	
	const char* getName() const override { return "Sound"; }
};

}

#endif // NEO_SOUNDBEHAVIOR_H
