#ifndef NEO_BGFXRENDERER_H
#define NEO_BGFXRENDERER_H

#include <Renderer.h>

namespace Neo 
{

class BGFXRenderer : public Renderer
{
public:
	void clear(float r, float g, float b, bool depth) override;
	void initialize(unsigned int w, unsigned int h, void* ndt, void* nwh) override;
	void swapBuffers() override;
};

}

#endif // NEO_BGFXRENDERER_H
