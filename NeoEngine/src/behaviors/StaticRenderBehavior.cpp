#include "behaviors/StaticRenderBehavior.h"

using namespace Neo;

REGISTER_BEHAVIOR(StaticRenderBehavior)

Behavior* StaticRenderBehavior::getNew() const
{
	return new StaticRenderBehavior; 
}

