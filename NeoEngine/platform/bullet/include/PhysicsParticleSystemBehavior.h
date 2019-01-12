#ifndef NEO_PHYSICSPARTICLESYSTEM_H
#define NEO_PHYSICSPARTICLESYSTEM_H

#include <PlatformParticleSystemBehavior.h>
#include <PlatformPhysicsContext.h>

namespace Neo 
{

class PhysicsParticleSystemBehavior : public Neo::PlatformParticleSystemBehavior
{
	float LifeTime = 5.0f;
	float LifeDivergence = 2.5f;
	
	float SpeedDivergence = 2.0f;
	Vector3 InitialSpeed = Vector3(0.0f, 0.0f, 0.0f);
	float Size = 5.0f;
	float Alpha = 1.0f;
	float AlphaDivergence = 0.0f;
	float SizeDivergence = 2.0f;
	float EmissionDelay = 0.0f;
	float EmissionDivergence = 0.1f;
	
public:
	PhysicsParticleSystemBehavior()
	{
		REGISTER_PROPERTY(LifeTime);
		REGISTER_PROPERTY(LifeDivergence);
		REGISTER_PROPERTY(InitialSpeed);
		REGISTER_PROPERTY(SpeedDivergence);
		REGISTER_PROPERTY(Size);
		REGISTER_PROPERTY(SizeDivergence);
		REGISTER_PROPERTY(Alpha);
		REGISTER_PROPERTY(AlphaDivergence);
		REGISTER_PROPERTY(EmissionDelay);
		REGISTER_PROPERTY(EmissionDivergence);
	}
	
	const char* getName() const override { return "PhysicsParticleSystemBehavior"; }
	Neo::Behavior* getNew() const override { return new PhysicsParticleSystemBehavior; }
	void copyTo(Behavior&) const override {}
	
	void begin(Neo::Platform& p, Neo::Renderer& render, Level& level) override;
	void end() override;
	void update(Platform& p, float dt) override;
	
	void serialize(std::ostream&) override;
	void deserialize(Level&, std::istream&) override;
	
private:
	struct Particle
	{
		Vector3 position;
		float size;
		float alpha;
		float spin;
		float time;
		bool alive;
	};
	
	struct PhysParticle
	{
		Particle particle;
		btRigidBody* btbody = nullptr;
		btCollisionShape* btshape = nullptr;
	};
	
	std::vector<PhysParticle> m_particles;
	
	PlatformPhysicsContext* m_physics = nullptr;

	float m_emissionTimer = 0.0f;
};

}

#endif // NEO_PHYSICSPARTICLESYSTEM_H
