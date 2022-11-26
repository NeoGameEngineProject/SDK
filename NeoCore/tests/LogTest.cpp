#include <gtest/gtest.h>
#include <Log.h>
#include <ThreadPool.h>

TEST(LogTest, Log)
{
	std::stringstream out;
	Neo::Log::setOutStream(out);
	
	LOG_INFO("HELLO WORLD");
	
	// The file location is compiler dependent, don't check it here.
	EXPECT_EQ(out.str().find("[ Info ] HELLO WORLD"), 0);
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
	
	std::string line;
	while (std::getline(out, line, '\n').good())
	{
		ASSERT_EQ(line.find("[ Info ] HELLO WORLD"), 0);
	}
}
