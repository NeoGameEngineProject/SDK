#include <Platform.h>

#include <iostream>
#include <InputContext.h>
#include <Level.h>
#include <behaviors/CameraBehavior.h>
#include <InputContext.h>

#include <HTMLView.h>

extern "C" int main()
{
	Neo::Platform platform;
	Neo::HTML::HTMLView htmlView;
	
	auto win = platform.createWindow(1024, 768);
	auto renderer = win->getRenderer();
	
	auto& input = platform.getInputContext();
	
	Neo::Level level;
	if(!level.load("assets/test.dae"))
	{
		std::cerr << "Could not load level!" << std::endl;
		return 1;
	}
	
	auto light = level.find("Lamp");
	auto camera = level.find("Camera");
	auto camBehavior = camera->getBehavior<Neo::CameraBehavior>();
	
	auto testCube = level.find("Test");
	
	level.setCurrentCamera(camBehavior);
	level.begin(platform, *renderer);
	
	Neo::Vector3 position = camera->getPosition();
	Neo::Vector3 rotation = camera->getRotation().getEulerAngles();
	
	htmlView.begin(win->getWidth(), win->getHeight());
	if(!htmlView.loadDocument("assets/test.html"))
		return 1;
	
	auto button1 = htmlView.getById("#button1");	
	htmlView.addCallback("button1", [&button1](const char* url) {
		static int counter = 0;	
		button1.setValue(("Button was pressed " + std::to_string(counter++) + " times!").c_str());
	});
	
	auto button2 = htmlView.getById("#button2");	
	htmlView.addCallback("button2", [&button2](const char* url) {
		static int counter = 1;
		button2.setValue(("Button was pressed " + std::to_string(counter) + " times!").c_str());
		counter *= 2;
	});
	
	auto button3 = htmlView.getById("#button3");	
	htmlView.addCallback("button3", [&button3](const char* url) {
		static int counter = 0;
		button3.setValue(("Button was pressed " + std::to_string(counter--) + " times!").c_str());
	});
	
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
		
		if(input.isKeyDown(Neo::KEY_W))
			light->translate(Neo::Vector3(0, 0, -1));
		else if(input.isKeyDown(Neo::KEY_S))
			light->translate(Neo::Vector3(0, 0, 1));
		
		if(input.isKeyDown(Neo::KEY_A))
			light->translate(Neo::Vector3(-1, 0, 0));
		else if(input.isKeyDown(Neo::KEY_D))
			light->translate(Neo::Vector3(1, 0, 0));
		
		if(input.getMouse().isKeyDown(Neo::MOUSE_BUTTON_LEFT) && !htmlView.isMouseOver(platform))
		{
			rotation.z -= input.getMouse().getDirection().x * 0.1;
			rotation.x -= input.getMouse().getDirection().y * 0.1;
		}
		
		camera->setPosition(position);
		camera->setRotation(Neo::Quaternion(rotation.x, rotation.y, rotation.z));
		
		testCube->rotate(Neo::Vector3(0, 0, 1), 2);
		
		level.update(platform, 0.0f);
		htmlView.update(platform, 0.0f);
		
		renderer->clear(57.0f/255.0f, 57.0f/255.0f, 57.0f/255.0f, true);
		level.draw(*renderer);
		htmlView.draw(*renderer);
		/*nvgBeginFrame(nvg, 1024, 768, 1.0f);
		nvgSave(nvg);
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, 20, 20, 100, 100, 5);
		nvgFillColor(nvg, nvgRGBA(11,11,11,0x66));
		nvgFill(nvg);
		nvgRestore(nvg);
		nvgEndFrame(nvg);*/
		
		renderer->swapBuffers();
	}
	
	htmlView.end();
	level.end();
	
	return 0;
}
