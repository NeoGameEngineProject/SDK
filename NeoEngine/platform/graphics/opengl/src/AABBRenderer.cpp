#include "AABBRenderer.h"
#include <Object.h>
#include <GL/glew.h>
#include <behaviors/CameraBehavior.h>

using namespace Neo;

const char* AABBRenderer::getName() const
{
	return "AABBRenderer";
}

Neo::Behavior* AABBRenderer::getNew() const
{
	return new AABBRenderer;
}

void AABBRenderer::copyTo(Neo::Behavior&) const { }

void AABBRenderer::begin(Neo::Renderer& render)
{
	PlatformRenderer* prender = reinterpret_cast<PlatformRenderer*>(&render);
	
	m_shader = prender->loadShader("assets/glsl/lines");
	assert(m_shader != -1);
	
	glGenVertexArrays(1, &m_vao);
	assert(m_vao != -1);
	
	glGenBuffers(1, &m_vbo);
	assert(m_vbo != -1);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
				sizeof(m_vertices),
				m_vertices,
				GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	
	glUseProgram(m_shader);
	m_uViewProjection = glGetUniformLocation(m_shader, "ViewProjection");
	assert(m_uViewProjection != -1);
	
	glUseProgram(0);
	glBindVertexArray(0);
}

void AABBRenderer::update(Neo::Platform& p, float dt)
{

}

void AABBRenderer::draw(Neo::Renderer& render)
{
	drawBox(render, getParent()->getTransformedBoundingBox());
}

void AABBRenderer::drawBox(Neo::Renderer& render, const Box3D& box)
{
	PlatformRenderer* prender = reinterpret_cast<PlatformRenderer*>(&render);
	auto* camera = prender->getCurrentCamera();
	const auto VP = camera->getProjectionMatrix() * camera->getViewMatrix();
	
	Vector3 points[] = {
				Vector3(box.min.x, box.min.y, box.min.z),
				Vector3(box.min.x, box.max.y, box.min.z),
				
				Vector3(box.min.x, box.max.y, box.min.z),
				Vector3(box.max.x, box.max.y, box.min.z),
				
				Vector3(box.max.x, box.max.y, box.min.z),
				Vector3(box.max.x, box.min.y, box.min.z),
				
				Vector3(box.max.x, box.min.y, box.min.z),
				Vector3(box.min.x, box.min.y, box.min.z),
				
				Vector3(box.min.x, box.min.y, box.max.z),
				Vector3(box.min.x, box.max.y, box.max.z),
				
				Vector3(box.min.x, box.max.y, box.max.z),
				Vector3(box.max.x, box.max.y, box.max.z),

				Vector3(box.max.x, box.max.y, box.max.z),
				Vector3(box.max.x, box.min.y, box.max.z),
			
			
				Vector3(box.min.x, box.min.y, box.min.z),
				Vector3(box.min.x, box.min.y, box.max.z),
				
				Vector3(box.min.x, box.max.y, box.min.z),
				Vector3(box.min.x, box.max.y, box.max.z),
				
				Vector3(box.max.x, box.min.y, box.min.z),
				Vector3(box.max.x, box.min.y, box.max.z),
				
				Vector3(box.max.x, box.max.y, box.min.z),
				Vector3(box.max.x, box.max.y, box.max.z)
	};
	
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
				sizeof(points),
				points,
				GL_DYNAMIC_DRAW);
	
	glUseProgram(m_shader);
	glUniformMatrix4fv(m_uViewProjection, 1, GL_FALSE, VP.entries);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	glLineWidth(3);
	
	glDrawArrays(GL_LINES, 0, sizeof(points)/sizeof(Vector3));
	glBindVertexArray(0);
	
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void AABBRenderer::end()
{

}
