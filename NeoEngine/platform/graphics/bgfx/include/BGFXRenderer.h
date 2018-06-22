#ifndef NEO_BGFXRENDERER_H
#define NEO_BGFXRENDERER_H

#include <Renderer.h>
#include <bgfx/bgfx.h>
#include <vector>

namespace Neo 
{

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
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		}
		
		bgfx::VertexDecl vertexStruct;
	} m_bgfxVertStruct;

	std::vector<bgfx::ProgramHandle> m_shaders;
	unsigned int m_screenWidth = 0, m_screenHeight = 0;
public:
	void clear(float r, float g, float b, bool depth) override;
	void initialize(unsigned int w, unsigned int h, void* ndt, void* nwh) override;
	void swapBuffers() override;
	void beginFrame(CameraBehavior& cam) override;
	void endFrame() override { swapBuffers(); }
	
	unsigned int loadShader(const char* path);
	bgfx::VertexDecl getVertexStruct() const { return m_bgfxVertStruct.vertexStruct; }
	bgfx::ProgramHandle getShader(size_t idx) 
	{
		return m_shaders[idx];
	}
};

}

#endif // NEO_BGFXRENDERER_H
