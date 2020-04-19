#ifndef NEO_PLATFORMPARTICLESYSTEMBEHAVIOR_H
#define NEO_PLATFORMPARTICLESYSTEMBEHAVIOR_H

#include <Behavior.h>
#include <Vector3.h>

namespace Neo
{

class Texture;

/// TODO Refactor into rendering and simulation part!
class NEO_ENGINE_EXPORT PlatformParticleSystemBehavior : public Neo::Behavior
{
protected:
	unsigned int ParticleCount = 64;
	FixedString<128> Texture = "assets/test.png";
	
	unsigned int m_shader = -1, m_vao = -1, m_vbo = -1;
	unsigned int m_uViewProjection = -1, m_uNormal = -1, m_uTime = -1;
	Neo::Texture* m_texture = nullptr;
	float m_currentTime = 0;

	struct ParticleVertex
	{
		Vector3 position;
		Vector3 color;
		Vector3 parameters;
	};
	
	Array<ParticleVertex> m_buffer;

public:
	PlatformParticleSystemBehavior()
	{
		REGISTER_PROPERTY(ParticleCount);
		// REGISTER_PROPERTY(Texture);
	}
	
	void begin(Neo::Platform& p, Neo::Renderer& render, Level& level) override;
	void end() override;
	void draw(Neo::Renderer& render) override;

	void updateParticleBuffers();
};

}

#endif // NEO_PLATFORMPARTICLESYSTEMBEHAVIOR_H
