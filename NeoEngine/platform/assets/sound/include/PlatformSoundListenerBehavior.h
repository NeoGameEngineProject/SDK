#ifndef NEO_PLATFORMSOUNDLISTENER_H
#define NEO_PLATFORMSOUNDLISTENER_H

#include <Behavior.h>

namespace Neo 
{

class PlatformSoundListenerBehavior : public Behavior
{
public:
	void update(Neo::Platform & p, float dt) override;
};

}

#endif
