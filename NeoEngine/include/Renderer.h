#ifndef NEO_RENDERER_H
#define NEO_RENDERER_H

namespace Neo 
{

class CameraBehavior;
class Level;

/**
 * @brief Represents a renderer for a specific platform.
 * 
 * Every renderer is bound to a Window.
 */
class Renderer
{
public:
	virtual ~Renderer() {}
	
	/**
	 * @brief Clear the screen.
	 * @param r Red component.
	 * @param g Green component.
	 * @param b Blue component.
	 * @param depth Whether the depth buffer needs to be cleared.
	 */
	virtual void clear(float r, float g, float b, bool depth) = 0;
	
	// ndt and nwh are platform specific window pointers. See SDL_SysWMinfo and bgfx::PlatformData
	/**
	 * @brief Initializes the renderer.
	 * 
	 * ndt and nwh are platform specific window pointers. See SDL_SysWMinfo and bgfx::PlatformData
	 * 
	 * @param w The window width.
	 * @param h The window height.
	 * @param ndt A platform specific window pointer.
	 * @param nwh A platform specific window pointer.
	 * @param ctx A platform specific graphics context (e.g. OpenGL context)
	 */
	virtual void initialize(unsigned int w, unsigned int h, void* ndt, void* nwh, void* ctx) = 0;
	
	/**
	 * @param Ends the frame and swaps the buffers.
	 */
	virtual void swapBuffers() = 0;
	
	/**
	 * @brief Set up the given level for rendering.
	 * @param level The level.
	 * @param camera The viewpoint camera.
	 */
	virtual void beginFrame(Level& level, CameraBehavior& camera) { beginFrame(camera); }
	
	/**
	 * @brief Set up a frame using the given camera.
	 * @param camera The viewpoint camera.
	 */
	virtual void beginFrame(CameraBehavior& camera) = 0;

	/**
	 * @brief Ends the frame but does not swap it.
	 */
	virtual void endFrame() = 0;
};

}

#endif // NEO_RENDERER_H
