#include "SDLWindow.h"
#include <iostream>

#include <SDL_syswm.h>

using namespace Neo;

SDLWindow::SDLWindow(unsigned int w, unsigned int h) : Window(w, h)
{
	m_win = SDL_CreateWindow("Neo Game Engine", 
				 SDL_WINDOWPOS_CENTERED, 
				 SDL_WINDOWPOS_CENTERED, 
				 w, h, 
				SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	
	if(m_win == nullptr)
	{
		std::cerr << "Could not create window! " << SDL_GetError() << std::endl;
	}
	
#ifndef __EMSCRIPTEN__
	// TODO Configuration
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	m_context = SDL_GL_CreateContext(m_win);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	
	m_context = SDL_GL_CreateContext(m_win);
#endif
}

SDLWindow::~SDLWindow()
{
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_win);
}

void SDLWindow::activateRendering()
{
	SDL_GL_MakeCurrent(m_win, m_context);
}

void SDLWindow::setTitle(const char * title)
{
	SDL_SetWindowTitle(m_win, title);
}

void SDLWindow::swapBuffers()
{
	SDL_GL_SwapWindow(m_win);
}

void SDLWindow::setRenderer(std::unique_ptr<Renderer>&& renderer)
{
	void* ndt = nullptr, *nwh = nullptr;
	
#ifndef __EMSCRIPTEN__
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if(!SDL_GetWindowWMInfo(m_win, &wmi))
	{
		std::cerr << "Could not get window info!";
		exit(0);
	}

#if defined(__linux__)

	if(wmi.subsystem == SDL_SYSWM_WAYLAND)
	{
		ndt = wmi.info.wl.display;
		nwh = (void*)(uintptr_t)wmi.info.wl.surface;
	}
	else if(wmi.subsystem == SDL_SYSWM_X11)
	{
		ndt = wmi.info.x11.display;
		nwh = (void*)(uintptr_t)wmi.info.x11.window;
	}
	
#elif defined(__apple__)
	ndt = NULL;
	nwh = wmi.info.cocoa.window;
#elif defined(WIN32)
	ndt = NULL;
	nwh = wmi.info.win.window;
//#	elif STEAMLINK
//		ndt          = wmi.info.vivante.display;
//		nwh          = wmi.info.vivante.window;
#endif
#endif // __EMSCRIPTEN__
	Window::setRenderer(std::move(renderer), ndt, nwh, m_context);
}

float SDLWindow::getDPI()
{
	float dpi;
	int display = SDL_GetWindowDisplayIndex(m_win);
	
	SDL_GetDisplayDPI(display, &dpi, nullptr, nullptr);
	return dpi;
}

