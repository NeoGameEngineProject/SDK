#ifndef NEO_PLATFORMSTATICRENDERBEHAVIOR_H
#define NEO_PLATFORMSTATICRENDERBEHAVIOR_H

#include <Behavior.h>
#include <Array.h>

namespace Neo 
{

class MeshBehavior;
class PlatformStaticRenderBehavior : public Neo::Behavior
{
	Array<unsigned int> m_vaos;
	Array<unsigned int> m_vertexBuffers;
	Array<Array<unsigned int>> m_texcoordBuffers;
	Array<unsigned int> m_indexBuffers;
	MeshBehavior* m_mesh = nullptr;

public:
	const char* getName() const override { return "PlatformStaticRender"; }
	void begin(Neo::Platform & p, Neo::Renderer & render) override;
	void end() override;
	
	void draw(Neo::Renderer& render) override;
};

}

#endif // NEO_PLATFORMSTATICRENDERBEHAVIOR_H
