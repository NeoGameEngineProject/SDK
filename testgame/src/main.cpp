
#include <Platform.h>

#include <iostream>
#include <InputContext.h>
#include <Level.h>
#include <behaviors/CameraBehavior.h>
#include <behaviors/SoundBehavior.h>
#include <behaviors/RigidbodyPhysicsBehavior.h>
#include <behaviors/SkyboxBehavior.h>

#include <InputContext.h>

#include <VRContext.h>
#include <HTMLView.h>
#include <Game.h>
#include <SplashScreen.h>

#include <WinMain.hpp>
#include <Log.h>
#include <behaviors/MeshBehavior.h>

#include <AssimpScene.h>

class TestGame : public Neo::GameState
{
	Neo::HTML::HTMLView htmlView;
	Neo::Level level;
	
	Neo::ObjectHandle light, camera, testCube;
	Neo::CameraBehavior* cameraBehavior;
	Neo::SoundBehavior sound;
	Neo::VRContext vr;
	
public:
	void begin(Neo::Platform & p, Neo::Window& w) override
	{
		vr.initialize();

		Neo::AssimpScene ascene;
		if(!ascene.loadFile(level, "assets/test.dae"))
		{
			std::cerr << "Could not load level!" << std::endl;
			exit(1);
		}
		
		light = level.find("Lamp");
		camera = level.find("Camera");
		cameraBehavior = camera->getBehavior<Neo::CameraBehavior>();
		
		auto* skybox = camera->addBehavior<Neo::SkyboxBehavior>();

		testCube = level.find("Test");
		
		level.find("Plane")->addBehavior<Neo::RigidbodyPhysicsBehavior>()->setMass(0);
		for(int i = 1; i <= 13; i++)
		{
			auto obj = level.find(("Cube" + std::to_string(i)).c_str());
			if(obj.empty())
			{
				LOG_WARNING("Could not find cube: Cube" << i);
				continue;
			}
			
			obj->addBehavior<Neo::RigidbodyPhysicsBehavior>()->setMass(i);
		}
	
		{
			auto torus = level.find("Torus");
			auto mesh = torus->getBehavior<Neo::MeshBehavior>();
			auto material = mesh->getMeshes()[0]->getMaterial();
			
			material.textures[Neo::Material::HEIGHT] = level.loadTexture("assets/textures/pattern_102/height.png");
						
			mesh->getMeshes()[0]->setMaterial(material);
		}

		{
			auto torus = level.find("ParallaxPlane");
			auto mesh = torus->getBehavior<Neo::MeshBehavior>();
			auto material = mesh->getMeshes()[0]->getMaterial();
			
			material.textures[Neo::Material::HEIGHT] = level.loadTexture("assets/textures/pattern_102/height.png");
						
			mesh->getMeshes()[0]->setMaterial(material);
		}

		
		level.setCurrentCamera(cameraBehavior);
		level.begin(p, *w.getRenderer());
	
		htmlView.begin(0, 0, w.getWidth(), w.getHeight(), w.getDPI());
	
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
		
		//sound = std::move(Neo::SoundBehavior(level.loadSound("assets/kansas.ogg")));
		//sound.initialize(getGame()->getPlatform());
		//sound.play();
		
		//getGame()->getPlatform().sleep(10000);
		//sound.stop();
	}
	
	void draw(Neo::Renderer& r) override
	{
		if(vr.hasVR())
		{
			vr.update();
			vr.enableEye(*level.getCurrentCamera(), Neo::VRContext::LEFT_EYE);
			level.draw(r);
			htmlView.draw(r);
			
			vr.enableEye(*level.getCurrentCamera(), Neo::VRContext::RIGHT_EYE);	
			level.draw(r);
			htmlView.draw(r);
			
			vr.endDraw(*level.getCurrentCamera());
		}
		else
		{
			// r.clear(57.0f/255.0f, 57.0f/255.0f, 57.0f/255.0f, true);
			level.draw(r, true);
			htmlView.draw(r);
			r.swapBuffers();
		}
	}
	
	void update(Neo::Platform & p, float dt) override
	{
		auto& input = p.getInputContext();
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
		
		// VR updates!
		auto& right = vr.getInputDevice(4);
		camera->translate(10*dt*Neo::Vector3(right.getAxis(Neo::VRContext::AXIS0_X), right.getAxis(Neo::VRContext::AXIS0_Y), 0));
		
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

extern "C" int main(int argc, char** argv)
{
	Neo::Game game(1024, 768, "Neo Test Game");

	auto testGame = std::make_unique<TestGame>();
	//auto splash = new Neo::States::SplashScreen(std::move(testGame), 2, 1, { "assets/Splash1.png", "assets/Splash2.png" });
	game.changeState(std::move(testGame)); // std::unique_ptr<Neo::States::SplashScreen>(splash));
	return game.run(argc, argv);
}
