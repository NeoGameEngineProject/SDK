#ifndef NEO_PLATFORMRENDERER_H
#define NEO_PLATFORMRENDERER_H

#include <Array.h>
#include <Renderer.h>
#include <Common.h>
#include <Mesh.h>

namespace Neo 
{

class Matrix4x4;
class Vector4;
class LightBehavior;
struct AABB;
class MeshBehavior;
class Material;

class PlatformRenderer : public Common
{
	unsigned int m_width, m_height;
	Array<LightBehavior*> m_visibleLights;
	Array<Vector4> m_lightBuffer;
	Array<unsigned char> m_buffer;
	
	size_t m_maxVisibleLights = 256;
	static const unsigned int MAX_LIGHTS_PER_OBJECT = 8;

	CameraBehavior* m_currentCamera = nullptr;

	unsigned int m_uModelView = -1, m_uModelViewProj = -1, m_uNormal = -1;

	unsigned int m_uMaterialDiffuse, m_uMaterialSpecular, m_uMaterialShininess, m_uMaterialOpacity, m_uMaterialEmit;
	unsigned int m_uDiffuseTexture, m_uNumTextures;
	unsigned int m_uTextureFlags[8];
	
	unsigned int m_uNumLights, m_uboLights;
	

public:
	void beginFrame(Level& level, CameraBehavior& cam) override;
	void beginFrame(Neo::CameraBehavior & camera) override;
	void clear(float r, float g, float b, bool depth) override;
	void endFrame() override;
	void initialize(unsigned int w, unsigned int h, void* ndt, void* nwh, void* ctx) override;
	void swapBuffers() override;
	void setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) override;

	void updateLights(MeshBehavior* mesh);
	void gatherLights(Array<LightBehavior*>& lights, MeshBehavior* mesh, unsigned short* buffer, unsigned short max, unsigned short& count);

	void setTransform(const Matrix4x4& transform);
	void setMaterial(MeshHandle mesh);
};

}

#endif // NEO_PLATFORMRENDERER_H
