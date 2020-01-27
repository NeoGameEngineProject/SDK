#ifndef NEO_RENDERER_H
#define NEO_RENDERER_H

#include "NeoEngine.h"

namespace Neo 
{

class CameraBehavior;
class Level;
class Object;
class Material;

/**
 * @brief Represents a renderer for a specific platform.
 * 
 * Every renderer is bound to a Window.
 */
class NEO_ENGINE_EXPORT Renderer
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
	
	/**
	 * @brief Sets the current graphics viewport.
	 * @param x The x origin.
	 * @param y The y origin.
	 * @param w The width.
	 * @param h The height.
	 */
	virtual void setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
	
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
	
	/**
	 * @brief Queues an object for drawing.
	 * 
	 * This allows the renderer to sort objects by material, depth or other criteria
	 * before rendering by queuing objects before actually rendering them.
	 *
	 * @param object The object to draw.
	 */
	virtual void draw(Object* object) = 0;
	
	virtual void setupMaterial(Material& material, const char* shaderName) = 0;
	virtual void compileShaders() = 0;
	
	unsigned int getDrawCallCount() const { return m_drawCallCount; }
	unsigned int getFaceCount() const { return m_faceCount; }
	void resetStatistics()
	{
		m_drawCallCount = 0;
		m_faceCount = 0;
	}
	
	void addDrawCall(unsigned int faces)
	{
		m_drawCallCount++;
		m_faceCount += faces;
	}
	
private:
	// Statistics
	unsigned int m_drawCallCount = 0, m_faceCount = 0;
};

}

#endif // NEO_RENDERER_H
