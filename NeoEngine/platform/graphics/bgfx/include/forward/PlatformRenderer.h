#ifndef NEO_PLATFORMRENDERER_H
#define NEO_PLATFORMRENDERER_H

#include <bgfx/bgfx.h>
#include <Common.h>
#include <Array.h>

namespace Neo 
{

class Vector4;
class LightBehavior;
struct AABB;
class MeshBehavior;

class PlatformRenderer : public BGFX::Common
{
	unsigned int m_width, m_height;
	Array<LightBehavior*> m_visibleLights;
	Array<Vector4> m_lightBuffer;
	Array<unsigned char> m_buffer;
	
	size_t m_maxVisibleLights = 256;
	static const unsigned int MAX_LIGHTS_PER_OBJECT = 8;
	
	bgfx::TextureHandle m_lightsTexture;
	bgfx::UniformHandle m_lightsTextureUniform, m_config;
	
	struct Light
	{
		bgfx::UniformHandle position, color, option, direction;
	} m_lightUniforms;
	
	CameraBehavior* m_currentCamera = nullptr;
	
public:
	
	void updateLights(MeshBehavior* mesh);
	void gatherLights(Array<LightBehavior*>& lights, MeshBehavior* mesh, unsigned short* buffer, unsigned short max, unsigned short& count);

	void initialize(unsigned int w, unsigned int h, void* backbuffer, void* ndt, void* nwh, void* ctx) override;
	void beginFrame(Level& level, CameraBehavior& cam) override;
	void beginFrame(Neo::CameraBehavior & camera) override;
	void clear(float r, float g, float b, bool depth) override;
	void endFrame() override;
	void swapBuffers() override;
	void setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) override;
	void compileShaders() override;
	void draw(Object* object);
	void setupMaterial(Neo::Material& material, const char* shaderName);
};

}

#endif // NEO_PLATFORMRENDERER_H
