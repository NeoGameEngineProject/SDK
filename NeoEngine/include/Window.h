#ifndef NEO_WINDOW_H
#define NEO_WINDOW_H

#include "NeoEngine.h"
#include <Renderer.h>
#include <memory>

namespace Neo
{

class NEO_ENGINE_EXPORT Window
{
	std::unique_ptr<Renderer> m_renderer;
	unsigned int m_width, m_height;
	
protected:
	void setDrawableSize(unsigned int w, unsigned int h) { m_width = w; m_height = h; }
	
public:
	Window(unsigned int w, unsigned int h) : m_width(w), m_height(h) {}
	virtual ~Window() {}
	virtual void setTitle(const char* title) = 0;
	virtual void activateRendering() = 0;
	virtual void swapBuffers() = 0;
	
	Renderer* getRenderer() { return m_renderer.get(); }
	
#ifndef SWIG
	void setRenderer(std::unique_ptr<Renderer>&& renderer, void* backbuffer, void* ndt, void* nwh, void* ctx) 
	{ 
		m_renderer = std::move(renderer); 
		m_renderer->initialize(m_width, m_height, backbuffer, ndt, nwh, ctx);
	}
	
	virtual void setRenderer(std::unique_ptr<Renderer>&& renderer) = 0;
#endif
	
	virtual float getDPI() { return 96.0f; }
	
	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
};

}

#endif // NEO_WINDOW_H
