#include <gtest/gtest.h>
#include <PhysStream.h>
#include <VFSFile.h>

#include <Log.h>

using namespace Neo;

TEST(PhysStream, Write)
{
	PHYSFS_init(nullptr);
	PHYSFS_setWriteDir("./");

	PhysStream out;
	out.open("test.txt", std::ios_base::out);

	ASSERT_TRUE(out.good());
	out << "TEST" << std::endl;
	out.close();
}

TEST(PhysStream, Read)
{
	
}
