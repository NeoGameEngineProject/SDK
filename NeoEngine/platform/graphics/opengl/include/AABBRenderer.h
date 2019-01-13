#ifndef NEO_AABBRENDERER_H
#define NEO_AABBRENDERER_H

#include <Behavior.h>
#include <Box3D.h>

namespace Neo 
{

class AABBRenderer : public Neo::Behavior
{
public:
	const char* getName() const override;
	Neo::Behavior* getNew() const override;
	void copyTo(Neo::Behavior& destination) const override;

	void begin(Neo::Platform& p, Neo::Renderer& render, Neo::Level& level) override { begin(render); }
	void update(Neo::Platform& p, float dt) override;
	void draw(Neo::Renderer& render) override;
	void end() override;
	
	void begin(Neo::Renderer& render);
	void drawBox(Neo::Renderer& render, const Box3D& box);
    
private:
	unsigned int m_shader = -1, m_vao = -1, m_vbo = -1;
	unsigned int m_uViewProjection = -1;
	Vector3 m_vertices[8];
};

}

#endif // NEO_AABBRENDERER_H
