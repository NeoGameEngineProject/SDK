#include "BGFXRenderer.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <FileTools.h>
#include <Texture.h>

#include <iostream>
#include <Object.h>
#include <behaviors/CameraBehavior.h>

using namespace Neo;

void BGFXRenderer::clear(float r, float g, float b, bool depth)
{
	unsigned char red = 255*r;
	unsigned char green = 255*g;
	unsigned char blue = 255*b;
	
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | (depth ? BGFX_CLEAR_DEPTH : 0),
			(red << 24) | (green << 16) | (blue << 8) | 0xff,
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
	
	bgfx::setViewRect(0, 0, 0, uint16_t(w), uint16_t(h));
	
	uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			// | BGFX_STATE_CULL_CW
			| BGFX_STATE_MSAA;
			
	bgfx::setState(state);
	
	loadShader("assets/glsl/base");
}

void BGFXRenderer::swapBuffers()
{
	// bgfx::touch(0);
	bgfx::frame();
}

void BGFXRenderer::beginFrame(CameraBehavior& cam)
{
	cam.enable(m_screenWidth, m_screenHeight);
	bgfx::setViewTransform(0, cam.getViewMatrix().entries, cam.getProjectionMatrix().entries);
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
	
	m_shaders.push_back(bgfx::createProgram(vertShader, fragShader));

	bgfx::destroy(vertShader);
	bgfx::destroy(fragShader);
	
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

