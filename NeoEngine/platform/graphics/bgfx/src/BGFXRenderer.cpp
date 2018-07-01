#include "BGFXRenderer.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <FileTools.h>
#include <Texture.h>

#include <iostream>
#include <Object.h>

#include <behaviors/CameraBehavior.h>
#include <behaviors/LightBehavior.h>
#include <Level.h>

using namespace Neo;

#define GEOMETRY_PASS 0
#define FULLSCREEN_PASS 1

void BGFXRenderer::clear(float r, float g, float b, bool depth)
{
	unsigned char red = 255*r;
	unsigned char green = 255*g;
	unsigned char blue = 255*b;
	
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | (depth ? BGFX_CLEAR_DEPTH : 0),
			(red << 24) | (green << 16) | (blue << 8) | 0x00,
			1.0f,
			0);
}

void BGFXRenderer::initialize(unsigned int w, unsigned int h, void* ndt, void* nwh)
{
	m_screenWidth = w;
	m_screenHeight = h;
	
	bgfx::Init init;
	init.type = bgfx::RendererType::OpenGL;
	init.resolution.width  = w;
	init.resolution.height = h;
	init.resolution.reset  = BGFX_RESET_VSYNC;
	
	bgfx::PlatformData pd;

	pd.ndt = ndt;
	pd.nwh = nwh;
	pd.context = NULL;
	pd.backBuffer = NULL;
	pd.backBufferDS = NULL;
	bgfx::setPlatformData(pd);
	bgfx::init(init);

	bgfx::setDebug(BGFX_DEBUG_NONE);
	
	bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(w), uint16_t(h));
	bgfx::setViewRect(FULLSCREEN_PASS, 0, 0, uint16_t(w), uint16_t(h));

	uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW;
			
	bgfx::setState(state);
	
	loadShader("assets/glsl/base");
	loadShader("assets/glsl/def_phong");
	
	// Set up framebuffers
	const uint32_t flags = 0
				| BGFX_TEXTURE_RT
				| BGFX_TEXTURE_MIN_POINT
				| BGFX_TEXTURE_MAG_POINT
				| BGFX_TEXTURE_MIP_POINT
				| BGFX_TEXTURE_U_CLAMP
				| BGFX_TEXTURE_V_CLAMP;
				
	m_gbufferTextures[0] = bgfx::createTexture2D(uint16_t(w), uint16_t(h), false, 1, bgfx::TextureFormat::BGRA8, flags);
	m_gbufferTextures[1] = bgfx::createTexture2D(uint16_t(w), uint16_t(h), false, 1, bgfx::TextureFormat::RGBA16F, flags);
	m_gbufferTextures[2] = bgfx::createTexture2D(uint16_t(w), uint16_t(h), false, 1, bgfx::TextureFormat::RGBA16F, flags);
	m_gbufferTextures[3] = bgfx::createTexture2D(uint16_t(w), uint16_t(h), false, 1, bgfx::TextureFormat::BGRA8,   flags);
	m_gbufferTextures[4] = bgfx::createTexture2D(uint16_t(w), uint16_t(h), false, 1, bgfx::TextureFormat::D24,   flags);
	m_gbuffer = bgfx::createFrameBuffer(5, m_gbufferTextures, true);
	
	bgfx::setViewFrameBuffer(GEOMETRY_PASS, m_gbuffer);

	bgfx::VertexDecl quadVertStruct;	
	quadVertStruct.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
	.end();

	// Setup quad
	static Vector2 vertices[8] = {
		Vector2(-1, -1), Vector2(0, 0),
		Vector2(-1, 1), Vector2(0, 1),
		Vector2(1, -1), Vector2(1, 0),
		Vector2(1, 1), Vector2(1, 1)
	};
	m_fullscreenQuad = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), quadVertStruct);
	
	static unsigned short indices[6] = {0, 1, 2, 1, 2, 3};
	m_fullscreenIndices = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
	
	m_gbufferTextureUniforms[0] = bgfx::createUniform("albedo", bgfx::UniformType::Int1);
	m_gbufferTextureUniforms[1] = bgfx::createUniform("normal", bgfx::UniformType::Int1);
	m_gbufferTextureUniforms[2] = bgfx::createUniform("position", bgfx::UniformType::Int1);
	m_gbufferTextureUniforms[3] = bgfx::createUniform("material", bgfx::UniformType::Int1);
	m_gbufferTextureUniforms[4] = bgfx::createUniform("depth", bgfx::UniformType::Int1);
	
	// Lights
	m_visibleLights.alloc(m_maxVisibleLights);
	
	m_lightBuffer.alloc(m_maxVisibleLights*5);
	memset(m_lightBuffer.data, 0, m_lightBuffer.count*sizeof(Vector4));
	
	m_lightsTexture = bgfx::createTexture2D(m_maxVisibleLights, 5, false, 1, bgfx::TextureFormat::RGBA32F, flags);
	m_lightsTextureUniform = bgfx::createUniform("lights", bgfx::UniformType::Int1);
	m_deferredConfig = bgfx::createUniform("deferredConfig", bgfx::UniformType::Vec4);
}

void BGFXRenderer::swapBuffers()
{
	// bgfx::touch(0);
	bgfx::frame();
}

void BGFXRenderer::beginFrame(Level& level, CameraBehavior& cam)
{
	beginFrame(cam);
	level.updateVisibility(cam, m_visibleLights);
	
	Vector4* lightPositions = reinterpret_cast<Vector4*>(m_lightBuffer.data);
	Vector4* lightColors = lightPositions + m_maxVisibleLights;
	Vector4* lightOptions = lightPositions + 2 * m_maxVisibleLights;
	Vector4* lightDirections = lightPositions + 3 * m_maxVisibleLights;
	
	Matrix4x4 MVP = cam.getViewMatrix();
	
	size_t i = 0;
	for(i = 0; i < m_lightBuffer.count; i++)
	{
		if(m_visibleLights[i] == nullptr)
			break;
		
		Object* parent = m_visibleLights[i]->getParent();

		lightPositions[i] = cam.getViewMatrix() * parent->getPosition();
		lightPositions[i].w = m_visibleLights[i]->attenuation;
		
		lightColors[i] = Vector4(m_visibleLights[i]->diffuse);
		lightColors[i].w = m_visibleLights[i]->brightness;
		
		lightOptions[i] = Vector4(m_visibleLights[i]->angle, m_visibleLights[i]->exponent, 0, 0);
		lightDirections[i] = (cam.getViewMatrix() * parent->getTransform() * Vector4(0, 0, -1, 0));
	}
	
	if(i)
	{
		bgfx::updateTexture2D(m_lightsTexture, 0, 0, 0, 0, m_maxVisibleLights, 5, bgfx::makeRef(m_lightBuffer.data, m_lightBuffer.count * sizeof(Vector4)));
		bgfx::touch(0);
		bgfx::frame();
	}
	
	Vector4 vec4(i);
	bgfx::setUniform(m_deferredConfig, &vec4);
}

void BGFXRenderer::beginFrame(CameraBehavior& cam)
{
	cam.enable(m_screenWidth, m_screenHeight);
	bgfx::setViewTransform(GEOMETRY_PASS, cam.getViewMatrix().entries, cam.getProjectionMatrix().entries);
	
	uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW;
	bgfx::setState(state);
}

void BGFXRenderer::endFrame()
{
	// 0.5 for dx
	bgfx::setState(0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_DEPTH_TEST_ALWAYS);
	
	for(int i = 0; i < 5; i++)
		bgfx::setTexture(i, m_gbufferTextureUniforms[i], m_gbufferTextures[i]);

	bgfx::setTexture(5, m_lightsTextureUniform, m_lightsTexture);
	
	bgfx::setVertexBuffer(0, m_fullscreenQuad);
	bgfx::setIndexBuffer(m_fullscreenIndices);
	bgfx::submit(FULLSCREEN_PASS, getShader(1));
}

unsigned int BGFXRenderer::loadShader(const char* path)
{
	std::string fullpath = path;
	
	unsigned int vertShaderSize = 0;
	char* vertShaderData = readBinaryFile((fullpath + "_vs.bin").c_str(), &vertShaderSize);
	auto vertShader = bgfx::createShader(bgfx::copy(vertShaderData, vertShaderSize));
	
	unsigned int fragShaderSize = 0;
	char* fragShaderData = readBinaryFile((fullpath + "_fs.bin").c_str(), &fragShaderSize);
	auto fragShader = bgfx::createShader(bgfx::copy(fragShaderData, fragShaderSize));
	
	auto program = bgfx::createProgram(vertShader, fragShader, true);
	m_shaders.push_back(program);
	
	delete vertShaderData;
	delete fragShaderData;
	
	return m_shaders.size() - 1;
}

size_t BGFXRenderer::createTexture(Texture* tex)
{
	const auto texRef = bgfx::makeRef(tex->getData(), tex->getStorageSize());
	auto format = bgfx::TextureFormat::RGBA8;
	if(tex->getComponents() == 3)
		format = bgfx::TextureFormat::RGB8;
	
	m_textures.push_back(bgfx::createTexture2D(tex->getWidth(), tex->getHeight(), tex->hasMipMap(), 1, format, 0, texRef));
	return m_textures.size() - 1;
}

