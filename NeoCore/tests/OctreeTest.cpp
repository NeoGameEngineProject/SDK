#include <gtest/gtest.h>
#include <Octree.h>
#include <Log.h>

using namespace Neo;

TEST(Octree, Test)
{
	const int SIZE = 64;
	Octree<float, 3> octree(Vector3(0, 0, 0), SIZE);
	
	octree.grow();
	
	for(int i = 0; i < SIZE; i++)
	{
		EXPECT_FALSE(!octree.insert(Vector3(i, i, i), Vector3(i/2, i/2, i/2), i));
	}
	
	EXPECT_FALSE(!octree.insert(Vector3(0, 1, 0), Vector3(1, 1, 1), 1));
	EXPECT_FALSE(!octree.insert(Vector3(0, 1, 0), Vector3(5, 5, 5), 2));
	EXPECT_FALSE(!octree.insert(Vector3(0, 1, 0), Vector3(128, 128, 128), 3));
	
	octree.remove(Vector3(0, 1, 0), 3);
	EXPECT_FALSE(!octree.update(Vector3(0, 1, 0), Vector3(1, 1, 0), Vector3(4, 4, 4), 2));
	
	std::vector<Vector3> found;
	octree.traverse(Vector3(0, 0, 0), Vector3(1, 1, 1), [&found](Octree<float>::Position* pos) {
		found.push_back(std::get<0>(*pos));
	});
	
	EXPECT_EQ(4ULL, found.size());
	EXPECT_TRUE(std::find(found.begin(), found.end(), Vector3(0, 0, 0)) != found.end());
	EXPECT_TRUE(std::find(found.begin(), found.end(), Vector3(1, 1, 1)) != found.end());
	EXPECT_TRUE(std::find(found.begin(), found.end(), Vector3(0, 1, 0)) != found.end());
	EXPECT_TRUE(std::find(found.begin(), found.end(), Vector3(1, 1, 0)) != found.end());
}

