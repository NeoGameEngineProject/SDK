#include <gtest/gtest.h>
#include <Log.h>
#include <ThreadPool.h>

TEST(LogTest, Log)
{
	std::stringstream out;
	Neo::Log::setOutStream(out);
	
	LOG_INFO("HELLO WORLD");
	
	EXPECT_EQ("[ Info ] HELLO WORLD in TestBody\n", out.str());
}

TEST(LogTest, ThreadLog)
{
	Neo::ThreadPool::start(4);

	std::stringstream out;
	Neo::Log::setOutStream(out);
	for(int i = 0; i < 5; i++)
		Neo::ThreadPool::schedule([] {
			for(int j = 0; j < 10; j++)
				LOG_INFO("HELLO WORLD");
			
			return 0;
		});
	
	Neo::ThreadPool::stop();
	
	std::stringstream reference;
	Neo::Log::setOutStream(reference);
	
	for(int i = 0; i < 50; i++)
		[]() { LOG_INFO("HELLO WORLD"); } (); // So we have it in operator() instead of TestBody
	
	EXPECT_EQ(reference.str(), out.str());
}
