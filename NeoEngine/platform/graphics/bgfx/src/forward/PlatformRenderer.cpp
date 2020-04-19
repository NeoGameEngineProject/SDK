#include "PlatformRenderer.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <Level.h>
#include <Vector4.h>
#include <behaviors/CameraBehavior.h>
#include <behaviors/LightBehavior.h>
#include <behaviors/MeshBehavior.h>

#include <iostream>

using namespace Neo;

void PlatformRenderer::beginFrame(Neo::CameraBehavior& camera)
{
	camera.enable(m_width, m_height);
	bgfx::setViewTransform(0, camera.getViewMatrix().entries, camera.getProjectionMatrix().entries);
}

void PlatformRenderer::beginFrame(Level& level, CameraBehavior& cam)
{
	m_currentCamera = &cam;
	beginFrame(cam);
	level.updateVisibility(cam, m_visibleLights);
	
	/*Matrix4x4 MVP = cam.getViewMatrix();
	
	Vector4* scratchpad = level.getScratchPad<Vector4>();
	assert(level.getScratchPadSize<Vector4>() > MAX_LIGHTS_PER_OBJECT);
	
	Vector4* position = scratchpad;
	Vector4* color = position + MAX_LIGHTS_PER_OBJECT;
	Vector4* option = color + MAX_LIGHTS_PER_OBJECT;
	Vector4* direction = option + MAX_LIGHTS_PER_OBJECT;
	
	size_t i = 0;
	for(i = 0; i < m_visibleLights.count && i < MAX_LIGHTS_PER_OBJECT; i++)
	{
		if(m_visibleLights[i] == nullptr)
			break;
		
		Object* parent = m_visibleLights[i]->getParent();
		
		position[i] = cam.getViewMatrix() * parent->getPosition();
		position[i].w = m_visibleLights[i]->attenuation;
		
		color[i] = Vector4(m_visibleLights[i]->diffuse);
		color[i].w = m_visibleLights[i]->brightness;

		
		option[i] = Vector4(m_visibleLights[i]->angle, m_visibleLights[i]->exponent, 0, 0);
		
		direction[i] = (cam.getViewMatrix() * parent->getTransform() * Vector4(0, 0, -1, 0));
	}
	
	bgfx::setUniform(m_lightUniforms.position, position, i);
	bgfx::setUniform(m_lightUniforms.color, color, i);
	bgfx::setUniform(m_lightUniforms.option, option, i);
	bgfx::setUniform(m_lightUniforms.direction, direction, i);
	
	Vector4 vec4(i);
	bgfx::setUniform(m_config, &vec4);*/
}

void PlatformRenderer::updateLights(MeshBehavior* mesh)
{
	assert(m_currentCamera);
	
	Vector4 scratchpad[MAX_LIGHTS_PER_OBJECT*5];
	unsigned short indices[MAX_LIGHTS_PER_OBJECT]; // Here we will gather all light indices into the m_visibleLights field
	unsigned short lightCount = 0;
	gatherLights(m_visibleLights, mesh, indices, MAX_LIGHTS_PER_OBJECT, lightCount);
	
	// Not required because gatherLights should ensure this, just to be sure we do it anyways
	assert(lightCount <= MAX_LIGHTS_PER_OBJECT);
	
	Vector4* position = scratchpad;
	Vector4* color = position + MAX_LIGHTS_PER_OBJECT;
	Vector4* option = color + MAX_LIGHTS_PER_OBJECT;
	Vector4* direction = option + MAX_LIGHTS_PER_OBJECT;
	
	size_t i = 0;
	for(i = 0; i < lightCount; i++)
	{
		auto light = m_visibleLights[indices[i]];
		if(light == nullptr)
			break;
		
		Object* parent = light->getParent();
		
		position[i] = m_currentCamera->getViewMatrix() * parent->getPosition();
		position[i].w = light->attenuation;
		
		color[i] = Vector4(light->diffuse);
		color[i].w = light->brightness;

		
		option[i] = Vector4(light->angle, light->exponent, 0, 0);
		
		direction[i] = (m_currentCamera->getViewMatrix() * parent->getTransform() * Vector4(0, 0, -1, 0));
	}
	
	bgfx::setUniform(m_lightUniforms.position, position, i);
	bgfx::setUniform(m_lightUniforms.color, color, i);
	bgfx::setUniform(m_lightUniforms.option, option, i);
	bgfx::setUniform(m_lightUniforms.direction, direction, i);
	
	Vector4 vec4(i);
	bgfx::setUniform(m_config, &vec4);
}

void PlatformRenderer::clear(float r, float g, float b, bool depth)
{
	unsigned char red = 255*r;
	unsigned char green = 255*g;
	unsigned char blue = 255*b;
	
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | (depth ? BGFX_CLEAR_DEPTH : 0),
			(red << 24) | (green << 16) | (blue << 8) | 0x00,
			1.0f,
			0);
}

void PlatformRenderer::endFrame()
{
	// Emscripten somehow shuffles the clear _after_ the render of the level
	// This prevents that by forcing a sync.
#ifdef __EMSCRIPTEN__
	bgfx::frame();
#endif
	
	m_currentCamera = nullptr;
}

void PlatformRenderer::initialize(unsigned int w, unsigned int h, void* backbuffer, void* ndt, void* nwh, void* ctx)
{
	m_width = w;
	m_height = h;
	
	// Init only once!
	static bool s_initialized = false;
	if(!s_initialized)
	{
		bgfx::Init init;
		
	#ifdef __EMSCRIPTEN__
		init.type = bgfx::RendererType::OpenGLES;
	#else
		init.type = bgfx::RendererType::OpenGL;
	#endif
		
		init.resolution.width  = w;
		init.resolution.height = h;
		init.resolution.reset  = BGFX_RESET_NONE;
		init.vendorId = BGFX_PCI_ID_NONE;

		bgfx::PlatformData pd;

		pd.nwh = nwh;
		pd.backBuffer = backbuffer;

		//pd.ndt = ndt;
		//pd.context = ctx;
		//pd.backBufferDS = nullptr;

		bgfx::setPlatformData(pd);
		bgfx::init(init);

		s_initialized = true;
	}
	
	bgfx::setViewMode(0, bgfx::ViewMode::DepthDescending);
	
	bgfx::setDebug(BGFX_DEBUG_NONE);
	bgfx::setViewRect(0, 0, 0, uint16_t(w), uint16_t(h));
	
#ifdef __EMSCRIPTEN__
	loadShader("assets/asm.js/phong");
#else
	loadShader("assets/glsl/phong");
#endif
	
	// Lights
	m_visibleLights.alloc(m_maxVisibleLights);
	m_config = bgfx::createUniform("u_config", bgfx::UniformType::Vec4);

	m_lightUniforms.position = bgfx::createUniform("u_lightPositions", bgfx::UniformType::Vec4, MAX_LIGHTS_PER_OBJECT);
	m_lightUniforms.color = bgfx::createUniform("u_lightColors", bgfx::UniformType::Vec4, MAX_LIGHTS_PER_OBJECT);
	m_lightUniforms.direction = bgfx::createUniform("u_lightDirections", bgfx::UniformType::Vec4, MAX_LIGHTS_PER_OBJECT);
	m_lightUniforms.option = bgfx::createUniform("u_lightOptions", bgfx::UniformType::Vec4, MAX_LIGHTS_PER_OBJECT);
}

void PlatformRenderer::swapBuffers()
{
	bgfx::frame();
}

void PlatformRenderer::gatherLights(Array<LightBehavior*>& lights, MeshBehavior* mesh, unsigned short* buffer, unsigned short max, unsigned short& count)
{
	for(size_t i = 0; i < lights.count && lights[i] != nullptr && count < max; i++)
	{
		auto parent = lights[i]->getParent();
		float distance = (mesh->getParent()->getPosition() - parent->getPosition()).getLength() - mesh->getBoundingBox().getDiameter() / 2.0f;
		float radius = sqrt(1.0f / (lights[i]->attenuation * 0.15f)); // 0.15 = Minimum brightness
		
		if(distance <= radius)
		{
			buffer[count] = i;
			count++;
		}
	}
}

void PlatformRenderer::setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{

}

void PlatformRenderer::compileShaders()
{

}

void PlatformRenderer::draw(Object* object)
{

}

void PlatformRenderer::setupMaterial(Neo::Material& material, const char* shaderName)
{

}
