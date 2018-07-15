#ifndef NEO_PLATFORMRENDERER_H
#define NEO_PLATFORMRENDERER_H

#include <Common.h>
#include <Array.h>
#include <Vector4.h>

#include <bgfx/bgfx.h>
#include <vector>

#include <cassert>

namespace Neo 
{

class PlatformRenderer : public BGFX::Common
{
	unsigned int m_screenWidth = 0, m_screenHeight = 0;
	
	bgfx::FrameBufferHandle m_gbuffer;
	bgfx::TextureHandle m_gbufferTextures[5];
	bgfx::UniformHandle m_gbufferTextureUniforms[5];
	bgfx::VertexBufferHandle m_fullscreenQuad;
	bgfx::IndexBufferHandle m_fullscreenIndices;
	
	Array<LightBehavior*> m_visibleLights;
	Array<Vector4> m_lightBuffer;
	
	size_t m_maxVisibleLights = 256;
	
	bgfx::TextureHandle m_lightsTexture;
	bgfx::UniformHandle m_lightsTextureUniform, m_deferredConfig;
	
public:
	void clear(float r, float g, float b, bool depth) override;
	void initialize(unsigned int w, unsigned int h, void* ndt, void* nwh) override;
	void swapBuffers() override;
	void beginFrame(Level& level, CameraBehavior& cam) override;
	void beginFrame(CameraBehavior& cam) override;
	void endFrame() override;
};

}

#endif // NEO_PLATFORMRENDERER_H
