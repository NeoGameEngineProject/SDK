#ifndef NEO_BGFXRENDERER_H
#define NEO_BGFXRENDERER_H

#include <Renderer.h>
#include <bgfx/bgfx.h>
#include <vector>

#include <cassert>

namespace Neo 
{

class Texture;
class BGFXRenderer : public Renderer
{
	struct BGFXVertexStruct
	{
		BGFXVertexStruct()
		{
			vertexStruct
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
				.end();
		}
		
		bgfx::VertexDecl vertexStruct;
	} m_bgfxVertStruct;
	
	struct BGFXTexCoordStruct
	{
		BGFXTexCoordStruct(unsigned int num)
		{
			auto mode = bgfx::Attrib::TexCoord0 + num;
			vertexStruct
				.begin()
				.add((bgfx::Attrib::Enum) mode, 2, bgfx::AttribType::Float)
				.end();
		}
		
		bgfx::VertexDecl vertexStruct;
	};

	BGFXTexCoordStruct m_texCoordStructs[4] = {
		BGFXTexCoordStruct(0),
		BGFXTexCoordStruct(1),
		BGFXTexCoordStruct(2),
		BGFXTexCoordStruct(3)
	};
	
	std::vector<bgfx::ProgramHandle> m_shaders;
	std::vector<bgfx::TextureHandle> m_textures;
	unsigned int m_screenWidth = 0, m_screenHeight = 0;
public:
	void clear(float r, float g, float b, bool depth) override;
	void initialize(unsigned int w, unsigned int h, void* ndt, void* nwh) override;
	void swapBuffers() override;
	void beginFrame(CameraBehavior& cam) override;
	void endFrame() override { swapBuffers(); }
	
	unsigned int loadShader(const char* path);
	bgfx::VertexDecl getVertexStruct() const { return m_bgfxVertStruct.vertexStruct; }
	bgfx::VertexDecl getTexCoordStruct() const { return m_texCoordStructs[0].vertexStruct; }

	bgfx::ProgramHandle getShader(size_t idx) 
	{
		return m_shaders[idx];
	}
	
	bgfx::TextureHandle getTexture(size_t id) { return m_textures[id]; }
	size_t createTexture(Texture* tex);
};

}

#endif // NEO_BGFXRENDERER_H
