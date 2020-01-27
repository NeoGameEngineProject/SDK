#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <Window.h>
#include <memory>

#include <SDLInputContext.h>
#include <PlatformRenderer.h>
#include <OpenALContext.h>

#include "NeoEngine.h"

namespace Neo
{

class NEO_ENGINE_EXPORT Platform
{
	SDLInputContext m_inputContext;
	OpenALContext m_soundContext;
	
public:
	Platform(const char* soundDevice = nullptr);
	~Platform() = default;
	
#ifndef SWIG
	std::unique_ptr<Window> createWindow(unsigned int w, unsigned int h, const char* title);
	std::unique_ptr<Renderer> createRenderer() { return std::make_unique<PlatformRenderer>(); }
#endif
	
	InputContext& getInputContext() { return m_inputContext; }
	
	void showCursor(bool value);
	unsigned long getTime();
	void sleep(long int millis);
	
	SoundContext& getSoundContext() { return m_soundContext; }
};

}

#endif
