#include <gtest/gtest.h>

#include <Level.h>
#include <LevelLoader.h>
#include <behaviors/MeshBehavior.h>

using namespace Neo;

TEST(LevelInstantiate, CopyMesh)
{
	Level level;
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae"));
	
	auto cube = level.find("Cube");
	ASSERT_FALSE(cube.empty());
	
	auto cube2 = level.instantiate("Cube2", *cube);
	ASSERT_FALSE(cube2.empty());
	
	EXPECT_NE(nullptr, cube2->getBehavior<MeshBehavior>());
}
