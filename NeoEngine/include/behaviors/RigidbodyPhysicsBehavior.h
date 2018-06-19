#ifndef NEO_RIGIDBODYPHYSICSBEHAVIOR_H
#define NEO_RIGIDBODYPHYSICSBEHAVIOR_H

#include <Behavior.h>

namespace Neo 
{

class RigidbodyPhysicsBehavior : public Neo::Behavior
{
public:
	const char* getName() const override { return "RigidbodyPhysics"; }
};

}

#endif // NEO_RIGIDBODYPHYSICSBEHAVIOR_H
