#include <Platform.h>
#include <SDLWindow.h>

#include <iostream>

#include <SDL2/SDL.h>

using namespace Neo;

Platform::Platform()
{
	if(SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "Could not initialize SDL2!" << std::endl;
		exit(1);
	}
}

Platform::~Platform()
{
	
}

std::unique_ptr<Window> Platform::createWindow(unsigned int w, unsigned int h)
{
	auto window = std::make_unique<SDLWindow>(w, h);
	window->setRenderer(createRenderer());
	return std::move(window);
}

void Platform::showCursor(bool value)
{
	SDL_ShowCursor(value);
}


unsigned long Platform::getTime()
{
	return SDL_GetTicks();
}

void Platform::sleep(long int millis)
{
	if(millis > 0)
		SDL_Delay(millis);
}
