#include "behaviors/RigidbodyPhysicsBehavior.h"

using namespace Neo;

REGISTER_BEHAVIOR(RigidbodyPhysicsBehavior)

const char* RigidbodyPhysicsBehavior::getName() const { return "RigidbodyPhysics"; }
Behavior* RigidbodyPhysicsBehavior::getNew() const
{
	return new RigidbodyPhysicsBehavior();
}
