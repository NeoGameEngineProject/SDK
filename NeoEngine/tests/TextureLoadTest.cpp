#include <gtest/gtest.h>

#include <Texture.h>
#include <TextureLoader.h>

using namespace Neo;

TEST(LoadTexture, Load)
{
	Texture tex;
	ASSERT_TRUE(TextureLoader::load(tex, "assets/test.png"));
}
