#include <behaviors/ParticleSystemBehavior.h>
#include <Object.h>

#include <algorithm>

using namespace Neo;

REGISTER_BEHAVIOR(ParticleSystemBehavior)

// TODO Helper function somewhere!
#define RANDOM (0.5f - 2.0f*static_cast<float>(rand()) / static_cast<float>(RAND_MAX))

void ParticleSystemBehavior::updateParticles(float dt)
{
	m_currentTime += dt;

	 // Mark dead particles for deletion
	for(int i = 0; i < m_particles.size(); i++)
	{
		auto* particle = &m_particles[i];
		if(particle->time <= m_currentTime)
			particle->alive = false;
	}

	// Delete dead particles
	m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](const Particle& p) { return !p.alive; }), m_particles.end()); 
	
	// Only emit if the timer ran out
	if(m_currentTime >= m_emissionTimer)
	{
		// Create missing particles
		Particle newParticle;
		Vector3 divergence;
		const Vector3 parentPosition = getParent()->getPosition();
		
		for(int i = m_particles.size(); i < ParticleCount; i++)
		{
			divergence.x = RANDOM * SpeedDivergence;
			divergence.y = RANDOM * SpeedDivergence;
			divergence.z = RANDOM * SpeedDivergence;

			newParticle.alive = true;
			newParticle.time = m_currentTime + LifeTime + RANDOM * LifeDivergence;
			newParticle.speed = InitialSpeed + divergence;
			newParticle.size = Size + RANDOM * SizeDivergence;
			newParticle.alpha = Alpha + RANDOM * AlphaDivergence;
			newParticle.position = parentPosition;
			m_particles.push_back(newParticle);
		}
	
		m_emissionTimer = m_currentTime + EmissionDelay + RANDOM*EmissionDivergence;
	}
	
	stepSimulation(dt);
}

void ParticleSystemBehavior::stepSimulation(float dt)
{
	Box3D aabb;
	aabb.min = m_particles[0].position;
	aabb.max = m_particles[0].position;
	
	for(int i = 0; i < m_particles.size(); i++)
	{
		auto* particle = &m_particles[i];
		particle->position += particle->speed * dt;
		particle->speed += Gravity * dt;
		
		m_buffer[i].position = particle->position;
		m_buffer[i].parameters = Vector3(particle->size, particle->alpha, particle->spin);
		
		aabb.min = Vector3(
			std::min(aabb.min.x, particle->position.x),
			std::min(aabb.min.y, particle->position.y),
			std::min(aabb.min.z, particle->position.z)
		);
		
		aabb.max = Vector3(
			std::max(aabb.max.x, particle->position.x),
			std::max(aabb.max.y, particle->position.y),
			std::max(aabb.max.z, particle->position.z)
		);
	}
	
	// auto* parent = getParent();
	// parent->setBoundingBox(aabb);
	// parent->setDirty(true);
}

void ParticleSystemBehavior::serialize(std::ostream& out)
{
}

void ParticleSystemBehavior::deserialize(Level&, std::istream& in)
{
}
