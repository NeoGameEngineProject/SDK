#include <gtest/gtest.h>
#include <ThreadPool.h>

using namespace Neo;

TEST(ThreadPoolTest, StartStop)
{
	ThreadPool::start(4);
	ThreadPool::stop();
	
	ThreadPool::start(16);
	ThreadPool::stop();
	
	ThreadPool::start(1);
	ThreadPool::stop();
}


TEST(ThreadPoolTest, Test)
{
	ThreadPool::start(3);
	
	std::future<int> results[1000];
	
	for(int i = 0; i < 1000; i++)
	{
		results[i] = std::move(ThreadPool::schedule([](int counter) { return counter + 1; }, i));
	}

	ThreadPool::synchronize();
	
	for(int i = 0; i < 1000; i++)
	{
		EXPECT_EQ(i+1, results[i].get());
	}
	
	ThreadPool::stop();
}
