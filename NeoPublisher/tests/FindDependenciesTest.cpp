#include <gtest/gtest.h>

#include <Publisher.h>

using namespace Neo;

TEST(PublisherTest, Read)
{
	Publisher p;
	p.loadConfig("config.json");
	p.publish(".");
}
