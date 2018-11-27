#include "behaviors/RigidbodyPhysicsBehavior.h"

using namespace Neo;

REGISTER_BEHAVIOR(RigidbodyPhysicsBehavior)

Behavior* RigidbodyPhysicsBehavior::getNew() const
{
	return new RigidbodyPhysicsBehavior();
}
