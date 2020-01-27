#ifndef NEO_SOUNDCONTEXT_H
#define NEO_SOUNDCONTEXT_H

#include <Sound.h>

namespace Neo
{

class NEO_ENGINE_EXPORT SoundContext
{
public:
	virtual void initialize(const char* device) = 0;
};

}

#endif // NEO_SOUNDCONTEXT_H
