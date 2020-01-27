#ifndef NEO_PHYSICSCONTEXT_H
#define NEO_PHYSICSCONTEXT_H

#include <PlatformPhysicsContext.h>

namespace Neo
{

class NEO_ENGINE_EXPORT PhysicsContext : public PlatformPhysicsContext
{
	float m_timeMultiplier = 1.0f;
	bool m_enabled = true;
public:
	float getTimeMultiplier() const { return m_timeMultiplier; }
	void setTimeMultiplier(float time) { m_timeMultiplier = time; }
	
	void setEnabled(bool v) { m_enabled = v; }
	bool isEnabled() const { return m_enabled; }
	
	void update(float dt) override
	{
		if(m_enabled)
			PlatformPhysicsContext::update(dt * m_timeMultiplier);
	}
};

}

#endif // NEO_PHYSICSCONTEXT_H
