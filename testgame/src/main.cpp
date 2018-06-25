#include <Platform.h>

#include <iostream>
#include <InputContext.h>
#include <Level.h>
#include <behaviors/CameraBehavior.h>
#include <InputContext.h>

extern "C" int main()
{
	Neo::Platform platform;
	auto win = platform.createWindow(1024, 768);
	auto renderer = win->getRenderer();
	
	auto& input = platform.getInputContext();
	
	Neo::Level level;
	level.load("assets/test.dae");
	
	auto camera = level.find("Camera");
	auto camBehavior = camera->getBehavior<Neo::CameraBehavior>();
	
	auto testCube = level.find("Test");
	
	level.setCurrentCamera(camBehavior);
	level.begin(platform, *renderer);
	
	Neo::Vector3 position = camera->getPosition();
	Neo::Vector3 rotation = camera->getRotation().getEulerAngles();
	
	while(1)
	{
		input.handleInput();
		if(input.isKeyDown(Neo::KEY_UP_ARROW))
			position += camera->getTransform().getRotatedVector3(Neo::Vector3(0, 0, -1));
		else if(input.isKeyDown(Neo::KEY_DOWN_ARROW))
			position += camera->getTransform().getRotatedVector3(Neo::Vector3(0, 0, 1));
		
		if(input.isKeyDown(Neo::KEY_LEFT_ARROW))
			position += camera->getTransform().getRotatedVector3(Neo::Vector3(-1, 0, 0));
		else if(input.isKeyDown(Neo::KEY_RIGHT_ARROW))
			position += camera->getTransform().getRotatedVector3(Neo::Vector3(1, 0, 0));
		
		if(input.getMouse().isKeyDown(Neo::MOUSE_BUTTON_LEFT))
		{
			rotation.z -= input.getMouse().getDirection().x * 0.1;
			rotation.x -= input.getMouse().getDirection().y * 0.1;
		}
		
		camera->setPosition(position);
		camera->setRotation(Neo::Quaternion(rotation.x, rotation.y, rotation.z));
		
		testCube->rotate(Neo::Vector3(0, 0, 1), 2);
		
		level.update(platform, 0.0);
		renderer->clear(0.4f, 0.4f, 0.4f, true);
		level.draw(*renderer);
	}
	level.end();
	
	return 0;
}
