#include "PlatformRenderer.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <Level.h>
#include <Vector4.h>
#include <behaviors/CameraBehavior.h>
#include <behaviors/LightBehavior.h>

#include <iostream>

using namespace Neo;

void PlatformRenderer::beginFrame(Neo::CameraBehavior& camera)
{
	camera.enable(m_width, m_height);
	bgfx::setViewTransform(0, camera.getViewMatrix().entries, camera.getProjectionMatrix().entries);
}

void PlatformRenderer::beginFrame(Level& level, CameraBehavior& cam)
{
	beginFrame(cam);
	level.updateVisibility(cam, m_visibleLights);
	
	Matrix4x4 MVP = cam.getViewMatrix();
	
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
}

void PlatformRenderer::initialize(unsigned int w, unsigned int h, void * ndt, void * nwh)
{
	m_width = w;
	m_height = h;
	
	bgfx::Init init;
	
#ifdef __EMSCRIPTEN__
	init.type = bgfx::RendererType::OpenGLES;
#else
	init.type = bgfx::RendererType::OpenGL;
#endif
	
	init.resolution.width  = w;
	init.resolution.height = h;
	init.resolution.reset  = BGFX_RESET_VSYNC;
	
	bgfx::PlatformData pd;

	pd.ndt = ndt;
	pd.nwh = nwh;
	pd.context = nullptr;
	pd.backBuffer = nullptr;
	pd.backBufferDS = nullptr;
	bgfx::setPlatformData(pd);
	bgfx::init(init);
	
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
