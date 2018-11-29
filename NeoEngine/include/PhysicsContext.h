#ifndef NEO_PHYSICSCONTEXT_H
#define NEO_PHYSICSCONTEXT_H

#include <PlatformPhysicsContext.h>

namespace Neo
{

class PhysicsContext : public PlatformPhysicsContext
{
	float m_timeMultiplier = 1.0f;
public:
	float getTimeMultiplier() const { return m_timeMultiplier; }
	void setTimeMultiplier(float time) { m_timeMultiplier = time; }
	
	void update(float dt) override
	{
		PlatformPhysicsContext::update(dt * m_timeMultiplier);
	}
};

}

#endif // NEO_PHYSICSCONTEXT_H
