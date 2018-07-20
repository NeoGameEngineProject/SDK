#include <gtest/gtest.h>

#include <Level.h>
#include <LevelLoader.h>

using namespace Neo;

TEST(LevelRaycast, CastNoHit)
{
	Level level;
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae"));
	
	Vector3 hit;
	ObjectHandle object;
	EXPECT_FALSE(level.castRay(Vector3(0, 0, 0), Vector3(0, 0, 1), 0.1, &hit, &object));
	EXPECT_EQ(object, ObjectHandle());
}

TEST(LevelRaycast, CastHit)
{
	Level level;
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae"));
	
	auto cube = level.find("Test");
	ASSERT_NE(cube, ObjectHandle());
	
	Vector3 direction = cube->getPosition().getNormalized();
	
	Vector3 hit;
	ObjectHandle object;
	EXPECT_TRUE(level.castRay(Vector3(0, 0, 0), direction, 100.0f, &hit, &object));
	EXPECT_NE(object, ObjectHandle());
	EXPECT_EQ(object->getName(), std::string("Cube"));
}

TEST(LevelRaycast, CastHitObjectNull)
{
	Level level;
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae"));
	
	auto cube = level.find("Test");
	ASSERT_NE(cube, ObjectHandle());
	
	Vector3 direction = cube->getPosition().getNormalized();
	
	Vector3 hit;
	EXPECT_TRUE(level.castRay(Vector3(0, 0, 0), direction, 100.0f, &hit));
}

TEST(LevelRaycast, CastHitHitAndObjectNull)
{
	Level level;
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae"));
	
	auto cube = level.find("Test");
	ASSERT_NE(cube, ObjectHandle());
	
	Vector3 direction = cube->getPosition().getNormalized();
	EXPECT_TRUE(level.castRay(Vector3(0, 0, 0), direction, 100.0f));
}
