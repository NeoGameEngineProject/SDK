#include <Platform.h>

#include <iostream>
#include <InputContext.h>

extern "C" int main()
{
	Neo::Platform platform;
	auto win = platform.createWindow(1024, 768);
	auto renderer = win->getRenderer();
	
	while(1)
	{
		platform.getInputContext().handleInput();
		
		renderer->clear(0.8, 0.0, 0.8, true);
		renderer->swapBuffers();
	}
	
	return 0;
}
