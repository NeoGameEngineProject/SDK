#ifndef NEO_PARTICLESYSTEM_H
#define NEO_PARTICLESYSTEM_H

#include <PlatformParticleSystemBehavior.h>

namespace Neo
{

class NEO_ENGINE_EXPORT ParticleSystemBehavior : public Neo::PlatformParticleSystemBehavior
{
	Vector3 Gravity = Vector3(0.0f, 0.0f, -9.81f);
	
	float LifeTime = 2.0f;
	float LifeDivergence = 1.1f;
	
	float SpeedDivergence = 2.0f;
	Vector3 InitialSpeed = Vector3(0.0f, 0.0f, 0.0f);
	float Size = 125;
	float Alpha = 1.0f;
	float AlphaDivergence = 0.0f;
	float SizeDivergence = 20.0f;
	float EmissionDelay = 0.0f;
	float EmissionDivergence = 0.1f;
	
public:
	ParticleSystemBehavior()
	{
		REGISTER_PROPERTY(Gravity);
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
	
	const char* getName() const override { return "ParticleSystem"; }
	Neo::Behavior* getNew() const override { return new ParticleSystemBehavior; }
	void copyTo(Behavior&) const override {}
	
	void serialize(std::ostream&) override;
	void deserialize(Level&, std::istream&) override;
	
private:
	void stepSimulation(float dt);
	void updateParticles(float dt);
	
	struct Particle
	{
		Vector3 position;
		Vector3 speed;
		float size;
		float alpha;
		float spin;
		float time;
		bool alive;
	};
	
	std::vector<Particle> m_particles;
	float m_emissionTimer = 0.0f;
	float m_currentTime = 0;

};

}

#endif // NEO_PARTICLESYSTEM_H
