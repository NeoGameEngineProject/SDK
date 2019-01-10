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
	const std::string base(TextureBase.str());
	static const char* postfixes[] = {"up.png", "down.png", "left.png", "right.png", "front.png", "back.png"};
	
	for(unsigned short i = 0; i < 6; i++)
	{
		m_textures[i] = level.loadTexture((base + postfixes[i]).c_str());
		prender->createTexture(m_textures[i]);
		
		glBindTexture(GL_TEXTURE_2D, m_textures[i]->getID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	m_shader = prender->loadShader("assets/glsl/skybox");
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
	
	glUniform1i(glGetUniformLocation(m_shader, "Texture"), 0);
	
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
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	glActiveTexture(GL_TEXTURE0);
	for(unsigned short i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]->getID());
		glDrawArrays(GL_TRIANGLES, i*6, 6);
	}
	
	glDepthMask(GL_TRUE);
	
	glUseProgram(0);
	glBindVertexArray(0);
}
