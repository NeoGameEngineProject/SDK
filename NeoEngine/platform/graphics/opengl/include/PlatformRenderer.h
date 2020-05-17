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
class AABBRenderer;
class SkyboxBehavior;

class NEO_ENGINE_EXPORT PlatformRenderer : public Common
{
	unsigned int m_width, m_height;
	
	unsigned short m_visibleLightCount = 0;
	Array<LightBehavior*> m_visibleLights;
	Array<Vector4> m_lightBuffer;
	Array<unsigned char> m_buffer;
	
	size_t m_maxVisibleLights = 256;
	static const unsigned int MAX_LIGHTS_PER_OBJECT = 8;

	SkyboxBehavior* m_currentSkybox = nullptr;
	CameraBehavior* m_currentCamera = nullptr;
	unsigned int m_uboLights;
	
	unsigned int m_currentFBO = 0, m_pfxFBO = -1, m_pfxTexture = -1, m_pfxDepthTexture = -1, m_pfxVAO = -1, m_pfxVBO = -1, m_pfxUFrustum = -1, m_pfxTime = -1;
	unsigned int m_pfxShader = -1;
	unsigned long long m_startTime = 0;

	std::vector<Object*> m_opaqueObjects;
	
#ifdef DRAW_AABB
	AABBRenderer* m_aabbRenderer = nullptr;
#endif
public:
	CameraBehavior* getCurrentCamera() { return m_currentCamera; }
	unsigned long long getTime();
	void beginFrame(Level& level, CameraBehavior& cam) override;
	void beginFrame(Neo::CameraBehavior & camera) override;
	void clear(float r, float g, float b, bool depth) override;
	void endFrame() override;
	void initialize(unsigned int w, unsigned int h, void* backbuffer, void* ndt, void* nwh, void* ctx) override;
	void swapBuffers() override;
	void setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) override;
	void compileShaders() override;
	
	void updateLights(MeshBehavior* mesh);
	void gatherLights(Array<LightBehavior*>& lights, MeshBehavior* mesh, unsigned short* buffer, unsigned short max, unsigned short& count);

	void enableMaterial(Neo::Material& material, const Vector3& cameraPosition, const Neo::Matrix4x4& Model, const Neo::Matrix4x4& ModelView, const Neo::Matrix4x4& ModelViewProjection, const Neo::Matrix4x4& Normal) override;
	
	void draw(Object* object);
	
	void setBackbuffer(void* fbo) override { m_currentFBO = (long int) fbo; }
};

}

#endif // NEO_PLATFORMRENDERER_H
