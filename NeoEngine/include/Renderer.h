#ifndef NEO_RENDERER_H
#define NEO_RENDERER_H

namespace Neo 
{

class CameraBehavior;
class Level;
class Renderer
{
public:
	virtual void clear(float r, float g, float b, bool depth) = 0;
	// ndt and nwh are platform specific window pointers. See SDL_SysWMinfo and bgfx::PlatformData
	virtual void initialize(unsigned int w, unsigned int h, void* ndt, void* nwh) = 0;
	virtual void swapBuffers() = 0;
	
	virtual void beginFrame(Level& level, CameraBehavior& camera) { beginFrame(camera); }
	virtual void beginFrame(CameraBehavior& camera) = 0;

	virtual void endFrame() = 0;
};

}

#endif // NEO_RENDERER_H
