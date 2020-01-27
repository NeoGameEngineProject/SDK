#ifndef NEO_RIGIDBODYPHYSICSBEHAVIOR_H
#define NEO_RIGIDBODYPHYSICSBEHAVIOR_H

#include <Behavior.h>
#include <PlatformRigidbodyPhysicsBehavior.h>

namespace Neo 
{

class NEO_ENGINE_EXPORT RigidbodyPhysicsBehavior : public Neo::PlatformRigidbodyPhysicsBehavior
{
public:
	
	void copyTo(Behavior& b) const override {}
	const char* getName() const override;
	Behavior* getNew() const override;
};

}

#endif // NEO_RIGIDBODYPHYSICSBEHAVIOR_H
