#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <Window.h>
#include <memory>

#include <SDLInputContext.h>
#include <PlatformRenderer.h>
#include <OpenALContext.h>

namespace Neo
{

class Platform
{
	SDLInputContext m_inputContext;
	OpenALContext m_soundContext;
	
public:
	Platform(const char* soundDevice = nullptr);
	~Platform();
	
	std::unique_ptr<Window> createWindow(unsigned int w, unsigned int h, const char* title);
	InputContext& getInputContext() { return m_inputContext; }
	
	void showCursor(bool value);
	unsigned long getTime();
	void sleep(long int millis);
	
	std::unique_ptr<Renderer> createRenderer() { return std::make_unique<PlatformRenderer>(); }
	SoundContext& getSoundContext() { return m_soundContext; }
};

}

#endif
