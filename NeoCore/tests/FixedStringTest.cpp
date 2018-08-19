#include <gtest/gtest.h>
#include <FixedString.h>

using namespace Neo;

TEST(FixedString, AssignmentConstChar)
{
	const char* cstring = "This is a string!";
	FixedString<32> str;
	
	str = cstring;
	
	EXPECT_EQ(strlen(cstring), str.getLength());
	EXPECT_EQ(0, strcmp(cstring, str.str()));
}

TEST(FixedString, AssignmentStdString)
{
	std::string stdstring = "This is a string!";
	FixedString<32> str;
	
	str = stdstring;
	
	EXPECT_EQ(stdstring.size(), str.getLength());
	EXPECT_EQ(stdstring, str.str());
}

TEST(FixedString, AssignmentFixedString)
{
	FixedString<32> str1 = "This is a string!";
	FixedString<32> str2;
	
	str2 = str1;
	
	EXPECT_EQ(str1.getLength(), str2.getLength());
	EXPECT_EQ(0, strcmp(str1.str(), str2.str()));
}

TEST(FixedString, AssignmentFixedStringDifferentSize)
{
	FixedString<20> str1 = "This is a string!";
	FixedString<32> str2;
	
	str2 = str1;
	
	EXPECT_EQ(str1.getLength(), str2.getLength());
	EXPECT_EQ(0, strcmp(str1.str(), str2.str()));
}

TEST(FixedString, ComparisonConstChar)
{
	const char* str1 = "This is a string!";
	FixedString<32> str2;
	
	str2 = str1;
	
	EXPECT_TRUE(str2 == str1);
	EXPECT_TRUE(str1 == str2);
	
	EXPECT_FALSE(str2 != str1);
	EXPECT_FALSE(str1 != str2);
}

TEST(FixedString, ComparisonStdString)
{
	std::string str1 = "This is a string!";
	FixedString<32> str2;
	
	str2 = str1;
	
	EXPECT_TRUE(str2 == str1);
	EXPECT_TRUE(str1 == str2);
	
	EXPECT_FALSE(str2 != str1);
	EXPECT_FALSE(str1 != str2);
}

TEST(FixedString, ComparisonFixedString)
{
	FixedString<32> str1 = "This is a string!";
	FixedString<32> str2;
	
	str2 = str1;
	
	EXPECT_TRUE(str2 == str1);
	EXPECT_TRUE(str1 == str2);
	
	EXPECT_FALSE(str2 != str1);
	EXPECT_FALSE(str1 != str2);
}

TEST(FixedString, ComparisonFixedStringDifferentSize)
{
	FixedString<20> str1 = "This is a string!";
	FixedString<32> str2;
	
	str2 = str1;
	
	EXPECT_TRUE(str2 == str1);
	EXPECT_TRUE(str1 == str2);
	
	EXPECT_FALSE(str2 != str1);
	EXPECT_FALSE(str1 != str2);
}

TEST(FixedString, ConcatConstChar)
{
	const char* str1 = "This is a string!";
	FixedString<32> str2 = "Somestring! ";
	
	str2 += str1;
	
	EXPECT_TRUE(str2 == "Somestring! This is a string!");
}

TEST(FixedString, ConcatStdString)
{
	std::string str1 = "This is a string!";
	FixedString<32> str2 = "Somestring! ";
	
	str2 += str1;
	
	EXPECT_TRUE(str2 == "Somestring! This is a string!");
}

TEST(FixedString, ConcatFixedString)
{
	FixedString<32> str1 = "This is a string!";
	FixedString<32> str2 = "Somestring! ";
	
	str2 += str1;
	
	EXPECT_TRUE(str2 == "Somestring! This is a string!");
}

TEST(FixedString, ConcatFixedStringDifferentSize)
{
	FixedString<20> str1 = "This is a string!";
	FixedString<32> str2 = "Somestring! ";
	
	str2 += str1;
	
	EXPECT_TRUE(str2 == "Somestring! This is a string!");
}
