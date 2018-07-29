#ifndef NEO_SDLWINDOW_H
#define NEO_SDLWINDOW_H

#include <Window.h>
#include <SDL.h>

namespace Neo
{

class SDLWindow : public Window
{
	SDL_Window* m_win = nullptr;
	SDL_GLContext m_context = nullptr;
	
public:
	SDLWindow(unsigned int w, unsigned int h);
	~SDLWindow();
	
	void activateRendering() override;
	void setTitle(const char * title) override;
	void swapBuffers() override;
	
	void setRenderer(std::unique_ptr<Renderer>&& renderer) override;
	float getDPI() override;
};

}

#endif // NEO_SDLWINDOW_H
