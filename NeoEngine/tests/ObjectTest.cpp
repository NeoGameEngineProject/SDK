#include <gtest/gtest.h>
#include <Object.h>
#include <Level.h>

using namespace Neo;

class TestBehavior: public Behavior
{
public:
	const char* getName() const override { return "Test"; }
	static const char* getStaticName() { return "Test"; }
	
	Behavior* getNew() const override { return new TestBehavior(); }
	void copyTo(Behavior& destination) const {}
};

TEST(Object, AddBehaviorTemplate)
{
	Object object("Test");
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
	Object object("Test");
	auto behavior = object.addBehavior(std::make_unique<TestBehavior>());
	auto foundBehavior = object.getBehavior(TestBehavior::getStaticName());
	
	ASSERT_NE(nullptr, behavior);
	ASSERT_NE(nullptr, foundBehavior);
	EXPECT_EQ(foundBehavior, behavior);
	
	object.removeBehavior("Test");
	EXPECT_EQ(nullptr, object.getBehavior("Test"));
}

TEST(Object, SetParent)
{
	Level lvl;
	auto object = lvl.addObject("Test");
	auto parent = lvl.addObject("Parent");

	parent->setPosition(Vector3(1, 1, 1));
	parent->updateMatrix();
	
	object->setParent(parent);

	LOG_INFO(object->getPosition());

	EXPECT_LT((object->getPosition() - Vector3(-1, -1, -1)).getLength(), 0.0001);
}

