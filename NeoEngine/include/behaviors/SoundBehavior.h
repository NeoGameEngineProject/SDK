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
	
	bool isEditorVisible() const override { return false; }
	
	void copyTo(Behavior& b) const override {}
	const char* getName() const override { return "Sound"; }
	Behavior* getNew() const override { return new SoundBehavior; }
};

}

#endif // NEO_SOUNDBEHAVIOR_H
