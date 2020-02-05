#include <gtest/gtest.h>

#include <Level.h>
#include <LevelLoader.h>
#include <JsonScene.h>

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

TEST(LoadLevel, SaveLoad)
{
	Level level;
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae"));

	ASSERT_TRUE(level.saveBinary("test.nlv"));

	Level level2;
	ASSERT_TRUE(level2.loadBinary("test.nlv"));
}

TEST(LoadLevel, SaveLoadJson)
{
	Level level;
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae"));
	ASSERT_TRUE(LevelLoader::load(level, "assets/test.dae", "Cube"));

	JsonScene scene;

	ASSERT_TRUE(scene.save(level, "test.jlv"));

	Level level2;
	ASSERT_TRUE(scene.load(level2, "test.jlv"));

	ASSERT_TRUE(scene.save(level2, "test2.jlv"));
}

std::string encodeData(std::istream& in);
void decodeData(std::stringstream& out, const std::string& str);

TEST(LoadLevel, EncodeData)
{
	std::stringstream ss1, ss2;
	unsigned int num = 0xDEADBEEF, num2 = 0;

	ss1.write((char*) &num, sizeof(num));
	std::string encoded = encodeData(ss1);

	decodeData(ss2, encoded);
	ss2.read((char*) &num2, sizeof(num2));

	EXPECT_EQ(num, num2);
}
