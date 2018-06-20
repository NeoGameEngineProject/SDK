#ifndef _BEHAVIOR_H
#define _BEHAVIOR_H

#include "NeoEngine.h"
#include <memory>

namespace Neo
{

class NEO_ENGINE_EXPORT Behavior
{
public:
	virtual const char* getName() const = 0;
};

typedef std::unique_ptr<Behavior> BehaviorRef;
typedef std::shared_ptr<Behavior> BehaviorSharedRef;

}

#endif
