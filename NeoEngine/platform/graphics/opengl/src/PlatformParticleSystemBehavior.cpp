#include "PlatformParticleSystemBehavior.h"
#include "PlatformRenderer.h"

#include <Object.h>
#include <Level.h>

#include <behaviors/CameraBehavior.h>

#include <Log.h>
#include <GL/glew.h>

#include <algorithm>

using namespace Neo;

void PlatformParticleSystemBehavior::begin(Neo::Platform& p, Neo::Renderer& render, Level& level)
{
	PlatformRenderer* prender = reinterpret_cast<PlatformRenderer*>(&render);
	m_buffer.alloc(ParticleCount);
	
	m_shader = prender->loadShader("assets/glsl/particle");
	assert(m_shader != -1);
	
	glGenVertexArrays(1, &m_vao);
	assert(m_vao != -1);
	
	glGenBuffers(1, &m_vbo);
	assert(m_vbo != -1);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
				m_buffer.count*sizeof(Vector3),
				m_buffer.data,
				GL_DYNAMIC_DRAW);

	glUseProgram(m_shader);
	m_uViewProjection = glGetUniformLocation(m_shader, "ViewProjection");
	assert(m_uViewProjection != -1);
	
	m_uTime = glGetUniformLocation(m_shader, "Time");
	assert(m_uViewProjection != -1);
	
	glUniform1i(glGetUniformLocation(m_shader, "Texture"), 0);
	
	glUseProgram(0);
	glBindVertexArray(0);
	
	m_particles.reserve(ParticleCount);
	
	m_texture = level.loadTexture(Texture.str());
	assert(m_texture);
	
	prender->createTexture(m_texture);
}

void PlatformParticleSystemBehavior::end()
{
	glDeleteVertexArrays(1, &m_vao);
}

void PlatformParticleSystemBehavior::update(Platform& p, float dt)
{
	updateParticles(dt);
}

#define RANDOM (0.5f - 2.0f*static_cast<float>(rand()) / static_cast<float>(RAND_MAX))

void PlatformParticleSystemBehavior::updateParticles(float dt)
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

void PlatformParticleSystemBehavior::updateParticleBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
				m_buffer.count*sizeof(ParticleVertex),
				m_buffer.data,
				GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), nullptr);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*) sizeof(Vector3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*) (sizeof(Vector3)*2));
}

void PlatformParticleSystemBehavior::draw(Neo::Renderer& render)
{
	PlatformRenderer* prender = reinterpret_cast<PlatformRenderer*>(&render);
	auto* camera = prender->getCurrentCamera();
	const auto VP = camera->getProjectionMatrix() * camera->getViewMatrix();
	
	glUseProgram(m_shader);
	glBindVertexArray(m_vao);
	
	updateParticleBuffers();
	
	glUniformMatrix4fv(m_uViewProjection, 1, GL_FALSE, VP.entries);
	glUniform1f(m_uTime, m_currentTime);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getID());
	
	glEnable(GL_BLEND);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	
	glDrawArrays(GL_POINTS, 0, m_particles.size());
	
	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDisable(GL_BLEND);
}
