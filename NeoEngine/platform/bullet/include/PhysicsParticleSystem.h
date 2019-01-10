#ifndef NEO_PHYSICSPARTICLESYSTEM_H
#define NEO_PHYSICSPARTICLESYSTEM_H

#include <behaviors/ParticleSystemBehavior.h>
#include <PlatformPhysicsContext.h>

namespace Neo 
{

class PhysicsParticleSystem : public Neo::ParticleSystemBehavior
{
	PlatformPhysicsContext* m_physics = nullptr;
	
	struct PhysParticle
	{
		Particle particle;
		btRigidBody* btbody = nullptr;
		btCollisionShape* btshape = nullptr;
	};
	
	std::vector<PhysParticle> m_particles;
	
public:
	void begin(Neo::Platform& p, Neo::Renderer& render, Level& level) override;
	void end() override;
	void update(Platform& p, float dt) override;
};

}

#endif // NEO_PHYSICSPARTICLESYSTEM_H
