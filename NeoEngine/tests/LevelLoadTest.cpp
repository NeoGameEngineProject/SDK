#include <gtest/gtest.h>

#include <Level.h>
#include <LevelLoader.h>

#include <behaviors/MeshBehavior.h>
#include <behaviors/LightBehavior.h>
#include <behaviors/CameraBehavior.h>

using namespace Neo;

TEST(LoadLevel, Load)
{
	Level level;
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae"));
	
	auto cube = level.find("Cube");
	auto lamp = level.find("Lamp");
	auto camera = level.find("Camera");
	
	ASSERT_NE(ObjectHandle(), cube);
	ASSERT_NE(ObjectHandle(), lamp);
	ASSERT_NE(ObjectHandle(), camera);
	
	ASSERT_NE(nullptr, cube->getBehavior<MeshBehavior>());
	ASSERT_NE(nullptr, lamp->getBehavior<LightBehavior>());
	ASSERT_NE(nullptr, camera->getBehavior<CameraBehavior>());
	
	auto root = level.getRoot();
	ASSERT_NE(ObjectHandle(), root);
	EXPECT_NE(ObjectHandle(), root->find("Cube"));
}
