#include <Platform.h>
#include <InputContext.h>
#include <Level.h>

#include <behaviors/StaticRenderBehavior.h>
#include <behaviors/CameraBehavior.h>
#include <behaviors/SoundBehavior.h>
#include <behaviors/RigidbodyPhysicsBehavior.h>
#include <behaviors/SkyboxBehavior.h>
#include <behaviors/MeshBehavior.h>

#include <InputContext.h>
#include <Game.h>
#include <LevelGameState.h>

#include <WinMain.hpp>
#include <Log.h>

#include <ThreadPool.h>

#include <JsonScene.h>
#include <AssimpScene.h>
#include <BinaryScene.h>
#include <glTFScene.h>

#include <iostream>

using namespace Neo;

class TestGame : public Neo::LevelGameState
{
	ObjectHandle m_cameraObject;
public:
	TestGame(const char* levelFile)
	{
		auto& level = getLevel();
		LevelLoader::load(level, levelFile);

		m_cameraObject = level.addObject("ViewerCamera");
		auto* cam = m_cameraObject->addBehavior<CameraBehavior>();

		level.setCurrentCamera(cam);
		level.setEnableCulling(false);

		LOG_INFO("Loaded level with " << level.getObjects().size() << " objects!");
	}

	void begin(Neo::Platform & p, Neo::Window& w) override
	{
		LevelGameState::begin(p, w);
	}
	
	void draw(Neo::Renderer& r) override
	{
		LevelGameState::draw(r);
	}
	
	void update(Neo::Platform& p, float dt) override
	{
		auto& input = p.getInputContext();
		if(input.isKeyDown(Neo::KEY_UP_ARROW))
			m_cameraObject->translate(Neo::Vector3(0, 0, -1), true);
		else if(input.isKeyDown(Neo::KEY_DOWN_ARROW))
			m_cameraObject->translate(Neo::Vector3(0, 0, 1), true);
		
		if(input.isKeyDown(Neo::KEY_LEFT_ARROW))
			m_cameraObject->translate(Neo::Vector3(-1, 0, 0), true);
		else if(input.isKeyDown(Neo::KEY_RIGHT_ARROW))
			m_cameraObject->translate(Neo::Vector3(1, 0, 0), true);
		
		if(input.getMouse().isKeyDown(Neo::MOUSE_BUTTON_LEFT))
		{
			auto rotation = m_cameraObject->getRotation().getEulerAngles();
			rotation.z -= input.getMouse().getDirection().x * 0.1;
			rotation.x -= input.getMouse().getDirection().y * 0.1;
			m_cameraObject->setRotation(Neo::Quaternion(rotation.x, rotation.y, rotation.z));
		}

		LevelGameState::update(p, dt);
	}
	
	void end() override
	{
		LevelGameState::end();
	}
};

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		LOG_ERROR("No model file name given as an argument!");
		return 1;
	}

	//Neo::ThreadPool::start();
	
	Neo::BinaryScene binLoader;
	Neo::JsonScene jsonLoader;
	Neo::AssimpScene assimpLoader;
	Neo::glTFScene gltfLoader;

	Neo::LevelLoader::registerLoader(&binLoader);
	Neo::LevelLoader::registerLoader(&jsonLoader);
	Neo::LevelLoader::registerLoader(&gltfLoader);
	Neo::LevelLoader::registerLoader(&assimpLoader);

	Game game(1024, 768, "Model Viewer");
	game.changeState(std::make_unique<TestGame>(argv[1]));

	return game.run(argc, argv);
}
