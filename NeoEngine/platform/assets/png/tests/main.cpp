#include <gtest/gtest.h>

#include <Texture.h>
#include <TextureLoader.h>

using namespace Neo;

TEST(LoadTexture, Load)
{
	Texture tex;
	ASSERT_TRUE(TextureLoader::load(tex, "assets/test.png"));
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
