#include <Platform.h>

#include <iostream>
#include <InputContext.h>
#include <Level.h>
#include <LevelLoader.h>
#include <behaviors/CameraBehavior.h>
#include <InputContext.h>

extern "C" int main()
{
	Neo::Platform platform;
	auto win = platform.createWindow(1024, 768);
	auto renderer = win->getRenderer();
	
	auto& input = platform.getInputContext();
	
	Neo::Level level;
	Neo::LevelLoader::loadLevel(level, "assets/test.dae");
	
	auto camera = level.find("Camera");
	auto camBehavior = camera->getBehavior<Neo::CameraBehavior>();
	
	level.setCurrentCamera(camBehavior);	
	level.begin(platform, *renderer);
	
	Neo::Vector3 position = camera->getPosition();
	
	while(1)
	{
		input.handleInput();
		if(input.isKeyDown(Neo::KEY_UP_ARROW))
			position += Neo::Vector3(0, 0, 1);
		else if(input.isKeyDown(Neo::KEY_DOWN_ARROW))
			position += Neo::Vector3(0, 0, -1);
		
		if(input.isKeyDown(Neo::KEY_LEFT_ARROW))
			position += Neo::Vector3(1, 0, 0);
		else if(input.isKeyDown(Neo::KEY_RIGHT_ARROW))
			position += Neo::Vector3(-1, 0, 0);
		
		camera->setPosition(position);
			
		level.update(platform, 0.0);
		renderer->clear(0.0f, 0.0f, 0.0f, true);
		level.draw(*renderer);
	}
	level.end();
	
	return 0;
}
