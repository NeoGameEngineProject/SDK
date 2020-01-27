#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <Window.h>
#include <memory>

#include <PlatformRenderer.h>
#include <OpenALContext.h>

#include <InputContext.h>

namespace Neo
{

class NEO_ENGINE_EXPORT Platform
{
	InputContextDummy m_input;
	OpenALContext m_soundContext;
	
public:
	Platform(const char* soundDevice = nullptr);
	~Platform();
	
#ifndef SWIG
	std::unique_ptr<Window> createWindow(unsigned int w, unsigned int h, const char* title);
	std::unique_ptr<Renderer> createRenderer() { return std::make_unique<PlatformRenderer>(); }
#endif

	InputContext& getInputContext() { return m_input; }
	
	void showCursor(bool value);
	unsigned long getTime();
	void sleep(long int millis);
	
	SoundContext& getSoundContext() { return m_soundContext; }
};

}

#endif
