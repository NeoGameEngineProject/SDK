#include "BGFXRenderer.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

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
}

void BGFXRenderer::swapBuffers()
{
	bgfx::touch(0);
	bgfx::frame();
}
