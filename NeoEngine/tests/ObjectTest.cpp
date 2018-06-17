#include <gtest/gtest.h>
#include <Object.h>

using namespace Neo;

class TestBehavior: public Behavior
{
public:
	const char* getName() const override { return "Test"; }
	static const char* getStaticName() { return "Test"; }
};

TEST(Object, AddBehaviorTemplate)
{
	Object object;
	auto behavior = object.addBehavior<TestBehavior>();
	auto foundBehavior = object.getBehavior<TestBehavior>();
	
	ASSERT_NE(nullptr, behavior);
	ASSERT_NE(nullptr, foundBehavior);
	EXPECT_EQ(foundBehavior, behavior);
	
	object.removeBehavior<TestBehavior>();
	EXPECT_EQ(nullptr, object.getBehavior<TestBehavior>());
}

TEST(Object, AddBehavior)
{
	Object object;
	auto behavior = object.addBehavior(std::make_unique<TestBehavior>());
	auto foundBehavior = object.getBehavior(TestBehavior::getStaticName());
	
	ASSERT_NE(nullptr, behavior);
	ASSERT_NE(nullptr, foundBehavior);
	EXPECT_EQ(foundBehavior, behavior);
	
	object.removeBehavior("Test");
	EXPECT_EQ(nullptr, object.getBehavior("Test"));
}

