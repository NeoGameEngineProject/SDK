#include "PlatformSkyboxBehavior.h"
#include "PlatformRenderer.h"

#include <Level.h>
#include <GL/glew.h>

using namespace Neo;

namespace
{

struct Vertex
{
	Vector3 position;
	Vector2 texcoord;
};

Vertex s_vertices[] = {
	Vertex{Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f)},
	Vertex{Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f)},
	Vertex{Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f)},
	Vertex{Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f)},
	Vertex{Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 1.0f)},
	
	Vertex{Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 0.0f)},
	Vertex{Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)},
	Vertex{Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f)},
	Vertex{Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 0.0f)},

	Vertex{Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f)},
	Vertex{Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0f, 1.0f)},
	Vertex{Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f)},
	Vertex{Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0f, 0.0f)},
	Vertex{Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0f, 1.0f)},
	
	Vertex{Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f)},
	Vertex{Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)},
	Vertex{Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f)},
	Vertex{Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f)},
	
	Vertex{Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f)},
	Vertex{Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)},
	Vertex{Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 0.0f)},
	Vertex{Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f)},
	Vertex{Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 1.0f)},

	Vertex{Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0f, 0.0f)},
	Vertex{Vector3(1.0f, -1.0f, -1.0f),Vector2(1.0f, 1.0f)},
	Vertex{Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0f, 1.0f)},
	Vertex{Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f)},
	Vertex{Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0f, 0.0f)},
};
}

void PlatformSkyboxBehavior::begin(Neo::Platform&, Neo::Renderer& render, Level& level)
{
	PlatformRenderer* prender = reinterpret_cast<PlatformRenderer*>(&render);
	static const char* postfixes[] = {"right.png", "left.png", "up.png", "down.png", "front.png", "back.png"};

	glGenTextures(1, &m_cubeMap);
	assert(m_cubeMap != -1);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);

	for(unsigned short i = 0; i < 6; i++)
	{
		m_textures[i] = level.loadTexture((TextureBase + postfixes[i]).c_str());

		// TODO Support different formats!
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, m_textures[i]->getWidth(), m_textures[i]->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, m_textures[i]->getData());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	m_shader = prender->loadShader("assets/materials/builtin/skybox");
	assert(m_shader != -1);
	
	glGenVertexArrays(1, &m_vao);
	assert(m_vao != -1);
	
	glGenBuffers(1, &m_vbo);
	assert(m_vbo != -1);
	
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
				sizeof(s_vertices),
				s_vertices,
				GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(Vector3));
	
	glUseProgram(m_shader);
	m_uViewProjection = glGetUniformLocation(m_shader, "ViewProjection");
	assert(m_uViewProjection != -1);
	
	glUniform1i(glGetUniformLocation(m_shader, "Skybox"), 0);
	
	glUseProgram(0);
	glBindVertexArray(0);
}

void PlatformSkyboxBehavior::drawSky(PlatformRenderer* prender)
{
	auto* camera = prender->getCurrentCamera();
	auto VP = camera->getProjectionMatrix();
	const auto rotation = camera->getViewMatrix().getRotation();
	VP.rotate(rotation.getAxis(), rotation.getAngle());
	
	glUseProgram(m_shader);
	glBindVertexArray(m_vao);
	
	glUniformMatrix4fv(m_uViewProjection, 1, GL_FALSE, VP.entries);
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);
	
	bindSkybox(0);
	
	for(unsigned short i = 0; i < 6; i++)
	{
		glDrawArrays(GL_TRIANGLES, i*6, 6);
	}
	
	prender->addDrawCall(36/3);
	
	glDepthMask(GL_TRUE);
	
	glUseProgram(0);
	glBindVertexArray(0);
}

int PlatformSkyboxBehavior::bindSkybox(int sampler)
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glActiveTexture(GL_TEXTURE0 + sampler);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
	return sampler;
}
