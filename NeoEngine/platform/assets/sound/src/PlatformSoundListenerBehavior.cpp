#include "PlatformSoundListenerBehavior.h"
#include <OpenALContext.h>
#include <Object.h>

using namespace Neo;

void PlatformSoundListenerBehavior::update(Neo::Platform& p, float dt)
{
	auto sound = reinterpret_cast<OpenALContext*>(&p.getSoundContext());
	auto parent = getParent();
	
	alListenerfv(AL_POSITION, parent->getPosition());
	alListenerfv(AL_VELOCITY, 0); // TODO Determine velocity!
	
	Vector3 direction[2];
	direction[0] = parent->getTransform() * Vector3(0, 0, 1);
	direction[1] = parent->getTransform() * Vector3(0, 1, 0);
	
	alListenerfv(AL_ORIENTATION, &direction[0].x);
}
