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
	ASSERT_TRUE(LevelLoader::loadLevel(level, "assets/test.dae"));
	
	auto cube = level.find("Cube");
	auto lamp = level.find("Lamp");
	auto camera = level.find("Camera");
	
	ASSERT_NE(nullptr, cube);
	ASSERT_NE(nullptr, lamp);
	ASSERT_NE(nullptr, camera);
	
	ASSERT_NE(nullptr, cube->getBehavior<MeshBehavior>());
	ASSERT_NE(nullptr, lamp->getBehavior<LightBehavior>());
	ASSERT_NE(nullptr, camera->getBehavior<CameraBehavior>());
	
	auto root = level.getRoot();
	ASSERT_NE(nullptr, root);
	EXPECT_NE(nullptr, root->find("Cube"));
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
