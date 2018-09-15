#include <Platform.h>
#include <iostream>

#include <chrono>
#include <thread>

#include <Log.h>

using namespace Neo;

Platform::Platform(const char* soundDevice)
{
	m_soundContext.initialize(soundDevice);
}

Platform::~Platform()
{
	
}

std::unique_ptr<Window> Platform::createWindow(unsigned int w, unsigned int h, const char* title)
{
	LOG_WARNING("Creating a window is not supported with the dummy platform!");
	return nullptr;
}

void Platform::showCursor(bool value)
{
	
}


unsigned long Platform::getTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void Platform::sleep(long int millis)
{
	if(millis > 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

