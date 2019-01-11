#include "PhysicsParticleSystemBehavior.h"
#include <Level.h>

using namespace Neo;

REGISTER_BEHAVIOR(PhysicsParticleSystemBehavior)

void PhysicsParticleSystemBehavior::begin(Neo::Platform& p, Neo::Renderer& render, Level& level)
{
	m_physics = reinterpret_cast<PlatformPhysicsContext*>(&level.getPhysicsContext());
	PlatformParticleSystemBehavior::begin(p, render, level);
	
	m_particles.reserve(ParticleCount);
}

void PhysicsParticleSystemBehavior::end()
{
	auto* world = m_physics->getWorld();
	for(auto& p : m_particles)
	{
		world->removeRigidBody(p.btbody);
		delete p.btbody;
		delete p.btshape;
	}
	
	PlatformParticleSystemBehavior::end();
}

#define RANDOM (0.5f - 2.0f*static_cast<float>(rand()) / static_cast<float>(RAND_MAX))
void PhysicsParticleSystemBehavior::update(Platform& p, float dt)
{
	auto* world = m_physics->getWorld();
	m_currentTime += dt;

	 // Mark dead particles for deletion
	for(int i = 0; i < m_particles.size(); i++)
	{
		auto* particle = &m_particles[i];
		if(particle->particle.time <= m_currentTime)
			particle->particle.alive = false;
	}

	// Delete dead particles
	m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [world](PhysParticle& p) {
		if(!p.particle.alive)
		{
			world->removeRigidBody(p.btbody);
			delete p.btbody;
			delete p.btshape;
			
			return true;
		}
		
		return false;
	}), m_particles.end()); 
	
	// Only emit if the timer ran out
	if(m_currentTime >= m_emissionTimer)
	{
		// Create missing particles
		PhysParticle newParticle;
		Vector3 divergence;
		const Vector3 parentPosition = getParent()->getPosition();
		
		for(int i = m_particles.size(); i < ParticleCount; i++)
		{
			divergence.x = RANDOM * SpeedDivergence;
			divergence.y = RANDOM * SpeedDivergence;
			divergence.z = RANDOM * SpeedDivergence;

			Vector3 speed = (InitialSpeed + divergence)*0.1f;
			
			newParticle.particle.alive = true;
			newParticle.particle.time = m_currentTime + LifeTime + RANDOM * LifeDivergence;
			newParticle.particle.size = Size + RANDOM * SizeDivergence;
			newParticle.particle.alpha = Alpha + RANDOM * AlphaDivergence;
			newParticle.particle.position = parentPosition;
			
			newParticle.btbody = new btRigidBody(0.1f, nullptr, nullptr);
			newParticle.btbody->setSleepingThresholds(0.2f, 0.2f);
			newParticle.btbody->setUserPointer(nullptr);
			newParticle.btbody->getWorldTransform().setOrigin(btVector3(parentPosition.x, parentPosition.y, parentPosition.z));
			newParticle.btbody->applyCentralImpulse(btVector3(speed.x, speed.y, speed.z));
		
			newParticle.btshape = new btSphereShape(newParticle.particle.size*0.1f); 
			newParticle.btbody->setCollisionShape(newParticle.btshape);
			
			world->addRigidBody(newParticle.btbody);
			m_particles.push_back(newParticle);
		}
	
		m_emissionTimer = m_currentTime + EmissionDelay + RANDOM*EmissionDivergence;
	}
	
	for(unsigned int i = 0; i < m_particles.size(); i++)
	{
		const auto& particle = m_particles[i];
		const auto& pos = particle.btbody->getWorldTransform().getOrigin();
		m_buffer[i].position = Vector3(pos.x(), pos.y(), pos.z());
		m_buffer[i].parameters = Vector3(particle.particle.size*10.0f, particle.particle.alpha, particle.particle.spin);
	}
	
	updateParticleBuffers();
}

void PhysicsParticleSystemBehavior::serialize(std::ostream& out)
{
}

void PhysicsParticleSystemBehavior::deserialize(Level&, std::istream& in)
{
}
