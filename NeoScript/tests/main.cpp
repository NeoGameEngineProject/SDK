#include <gtest/gtest.h>
#include <LuaBehavior.h>

#include <fstream>
#include <Level.h>

using namespace Neo;

TEST(LuaScript, DoString)
{
	LuaScript lua;
	EXPECT_TRUE(lua.doString("print(\"Hello World\")"));
	EXPECT_TRUE(lua.doString("print(Neo.Vector3(1, 2, 3))"));
}

TEST(LuaScript, DoFile)
{
	LuaScript lua;
	
	std::ofstream out("test.lua");
	ASSERT_FALSE(!out);
	
	out << "print(\"Hello Lua World!\")";
	out.close();
	
	EXPECT_TRUE(lua.doFile("test.lua"));
}

TEST(LuaScript, DoFileFail)
{
	LuaScript lua;
	
	std::ofstream out("test.lua");
	ASSERT_FALSE(!out);
	
	out << "print(\"Hello Lua World!\") somegarbage lkjfsdf + f-fds+a+fasd";
	out.close();
	
	EXPECT_FALSE(lua.doFile("test.lua"));
}

TEST(LuaBehavior, SerializeDeserialize)
{
	std::ofstream out("test1.lua");
	ASSERT_FALSE(!out);
	
	out << "globvar = 123; globvar2 = 321; local locvar = 1444";
	out.close();
	
	out = std::ofstream("test2.lua");
	ASSERT_FALSE(!out);
	
	out << "globvar = 432124; globvar2 = 343221; function test() if globvar ~= 123 or globvar2 ~= 321 then nilvar = nilvar + nilvar end end";
	out.close();
	
	LuaBehavior lua1("test1.lua");
	LuaBehavior lua2("test2.lua");
	
	Level level;
	std::stringstream ss;
	lua1.serialize(ss);
	
	ss << "UNRELATED_GARBAGE";
	lua2.deserialize(level, ss);
	
	EXPECT_TRUE(lua2.getScript()("test"));
	
	std::string garbage;
	ss >> garbage;
	EXPECT_EQ("UNRELATED_GARBAGE", garbage);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
