#ifndef NEO_PLATFORMPARTICLESYSTEMBEHAVIOR_H
#define NEO_PLATFORMPARTICLESYSTEMBEHAVIOR_H

#include <Behavior.h>
#include <Vector3.h>

namespace Neo
{

class Texture;
class PlatformParticleSystemBehavior : public Neo::Behavior
{
	unsigned int ParticleCount = 512;
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
	
	FixedString<128> Texture = "assets/test.png";
	
	float m_emissionTimer = 0.0f;
	
	unsigned int m_shader = -1, m_vao = -1, m_vbo = -1;
	unsigned int m_uViewProjection = -1, m_uNormal = -1, m_uTime = -1;
	Neo::Texture* m_texture = nullptr;
	
	float m_currentTime = 0;
	
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

	struct ParticleVertex
	{
		Vector3 position;
		Vector3 color;
		Vector3 parameters;
	};
	
	std::vector<Particle> m_particles;
	Array<ParticleVertex> m_buffer;

public:
	PlatformParticleSystemBehavior()
	{
		REGISTER_PROPERTY(ParticleCount);
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
		// REGISTER_PROPERTY(Texture);
	}
	
	void begin(Neo::Platform& p, Neo::Renderer& render, Level& level) override;
	void end() override;
	
	void update(Platform& p, float dt) override;
	void draw(Neo::Renderer& render) override;
	
	void updateParticles(float dt);
	void updateParticleBuffers();
};

}

#endif // NEO_PLATFORMPARTICLESYSTEMBEHAVIOR_H
