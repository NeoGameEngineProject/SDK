#include <Platform.h>

#include <iostream>
#include <InputContext.h>
#include <Level.h>
#include <behaviors/CameraBehavior.h>
#include <InputContext.h>

#include <VFSFile.h>

#include <HTMLView.h>
#include <Game.h>
#include <SplashScreen.h>

class TestGame : public Neo::GameState
{
	Neo::HTML::HTMLView htmlView;
	Neo::Level level;
	
	Neo::Object* light, *camera, *testCube;
	Neo::CameraBehavior* cameraBehavior;
	
public:
	void begin(Neo::Platform & p, Neo::Window& w) override
	{
		if(!level.load("assets/test.dae"))
		{
			std::cerr << "Could not load level!" << std::endl;
			exit(1);
		}
		
		light = level.find("Lamp");
		camera = level.find("Camera");
		cameraBehavior = camera->getBehavior<Neo::CameraBehavior>();
		
		testCube = level.find("Test");
	
		level.setCurrentCamera(cameraBehavior);
		level.begin(p, *w.getRenderer());
		
		htmlView.begin(w.getWidth(), w.getHeight());
		if(!htmlView.loadDocument("assets/test.html"))
			exit(1);
		
		htmlView.addCallback("button1", [this](const char* url) {
			auto button1 = htmlView.getById("#button1");	
			static int counter = 0;	
			button1.setValue(("Button was pressed " + std::to_string(counter++) + " times!").c_str());
		});
		
		htmlView.addCallback("button2", [this](const char* url) {
			auto button2 = htmlView.getById("#button2");	
			static int counter = 1;
			button2.setValue(("Button was pressed " + std::to_string(counter) + " times!").c_str());
			counter *= 2;
		});
		
		htmlView.addCallback("button3", [this](const char* url) {
			auto button3 = htmlView.getById("#button3");	
			static int counter = 0;
			button3.setValue(("Button was pressed " + std::to_string(counter--) + " times!").c_str());
		});
	}
	
	void draw(Neo::Renderer& r) override
	{
		r.clear(57.0f/255.0f, 57.0f/255.0f, 57.0f/255.0f, true);
		level.draw(r);
		htmlView.draw(r);
		r.swapBuffers();
	}
	
	void update(Neo::Platform & p, float dt) override
	{
		auto& input = p.getInputContext();
		input.handleInput();

		if(input.isKeyDown(Neo::KEY_UP_ARROW))
			camera->translate(Neo::Vector3(0, 0, -1), true);
		else if(input.isKeyDown(Neo::KEY_DOWN_ARROW))
			camera->translate(Neo::Vector3(0, 0, 1), true);
		
		if(input.isKeyDown(Neo::KEY_LEFT_ARROW))
			camera->translate(Neo::Vector3(-1, 0, 0), true);
		else if(input.isKeyDown(Neo::KEY_RIGHT_ARROW))
			camera->translate(Neo::Vector3(1, 0, 0), true);
		
		if(input.isKeyDown(Neo::KEY_W))
			light->translate(Neo::Vector3(0, 0, -1));
		else if(input.isKeyDown(Neo::KEY_S))
			light->translate(Neo::Vector3(0, 0, 1));
		
		if(input.isKeyDown(Neo::KEY_A))
			light->translate(Neo::Vector3(-1, 0, 0));
		else if(input.isKeyDown(Neo::KEY_D))
			light->translate(Neo::Vector3(1, 0, 0));
		
		if(input.getMouse().isKeyDown(Neo::MOUSE_BUTTON_LEFT) && !htmlView.isMouseOver(p))
		{
			auto rotation = camera->getRotation().getEulerAngles();
			rotation.z -= input.getMouse().getDirection().x * 0.1;
			rotation.x -= input.getMouse().getDirection().y * 0.1;
			camera->setRotation(Neo::Quaternion(rotation.x, rotation.y, rotation.z));
		}
		
		testCube->rotate(Neo::Vector3(0, 0, 1), 20*dt);
		
		level.update(p, dt);
		htmlView.update(p, dt);
	}
	
	void end() override
	{
		htmlView.end();
		level.end();
	}
};

#ifndef ASSET_MODE
#define ASSET_MODE 0
#endif

extern "C" int main(int argc, char** argv)
{
#if ASSET_MODE != 0
	const char* pkg = (ASSET_MODE == 1 ? argv[0] : "data.neo");
	if(!Neo::VFSOpenHook::mount(pkg, argv[0]))
	{
		std::cerr << "Could not open assets file!" << std::endl;
		return 1;
	}
#endif
	
	Neo::Game game(1024, 768, "Neo Test Game");

	auto testGame = std::make_unique<TestGame>();
	auto splash = new Neo::States::SplashScreen(std::move(testGame), 2, 1, { "assets/Splash1.png", "assets/Splash2.png" });
	game.changeState(std::unique_ptr<Neo::States::SplashScreen>(splash));
	return game.run();
}
