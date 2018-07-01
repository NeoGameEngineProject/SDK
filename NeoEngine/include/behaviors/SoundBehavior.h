#ifndef NEO_SOUNDBEHAVIOR_H
#define NEO_SOUNDBEHAVIOR_H

#include <Behavior.h>

namespace Neo 
{

class SoundBehavior : public Neo::Behavior
{
public:
	const char* getName() const override { return "Sound"; }
};

}

#endif // NEO_SOUNDBEHAVIOR_H
