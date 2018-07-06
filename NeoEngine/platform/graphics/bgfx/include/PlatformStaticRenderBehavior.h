#ifndef NEO_PLATFORMSTATICRENDERBEHAVIOR_H
#define NEO_PLATFORMSTATICRENDERBEHAVIOR_H

#include <Behavior.h>
#include <bgfx/bgfx.h>
#include <Array.h>

namespace Neo 
{

class MeshBehavior;
class PlatformStaticRenderBehavior : public Neo::Behavior
{
	Array<bgfx::VertexBufferHandle> m_vertexBuffers;
	
	Array<Array<bgfx::VertexBufferHandle>> m_texcoordBuffers;

	Array<bgfx::IndexBufferHandle> m_indexBuffers;
	MeshBehavior* m_mesh = nullptr;
	
	bgfx::UniformHandle m_uMaterialDiffuse, m_uMaterialSpecular, m_uMaterialShininess, m_uMaterialOpacity, m_uMaterialEmit;
	bgfx::UniformHandle m_uDiffuseTexture;
	bgfx::UniformHandle m_uTextureConfig;
public:
	const char* getName() const override { return "PlatformStaticRender"; }
	void begin(Neo::Platform & p, Neo::Renderer & render) override;
	void end() override;
	
	void draw(Neo::Renderer& render) override;
};

}

#endif // NEO_PLATFORMSTATICRENDERBEHAVIOR_H
