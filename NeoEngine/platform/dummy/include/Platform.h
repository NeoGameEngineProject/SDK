#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <Window.h>
#include <memory>

#include <PlatformRenderer.h>
#include <OpenALContext.h>

#include <InputContext.h>

namespace Neo
{

class Platform
{
	InputContextDummy m_input;
	OpenALContext m_soundContext;
	
public:
	Platform(const char* soundDevice = nullptr);
	~Platform();
	
	std::unique_ptr<Window> createWindow(unsigned int w, unsigned int h, const char* title);
	InputContext& getInputContext() { return m_input; }
	
	void showCursor(bool value);
	unsigned long getTime();
	void sleep(long int millis);
	
	std::unique_ptr<Renderer> createRenderer() { return std::make_unique<PlatformRenderer>(); }
	SoundContext& getSoundContext() { return m_soundContext; }
};

}

#endif
