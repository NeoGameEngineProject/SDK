#include <gtest/gtest.h>

#include <Property.h>

using namespace Neo;

TEST(Property, SaveLoad)
{
	std::stringstream stream;
	
	StaticProperty<int> intProp("Name", 32);
	intProp.serialize(stream);

	IProperty* prop = deserializeProperty(stream);

	ASSERT_TRUE(prop);
	EXPECT_EQ(intProp.getName(), prop->getName());
	EXPECT_EQ(intProp.getType(), prop->getType());
	EXPECT_EQ(intProp.getSize(), prop->getSize());
	EXPECT_EQ(*intProp.get(), prop->get<int>());
}

TEST(PropertySystem, SaveLoad)
{
	std::stringstream stream;
	PropertySystem props, props2;

	props.registerProperty<int>("Test") = 32;
	props.registerProperty<int>("Test2") = 64;
	props.registerProperty<int>("Test3") = 128;
	
	props.serialize(stream);
	props2.deserialize(stream);

	ASSERT_EQ(props.getProperties().size(), props2.getProperties().size());
	for(int i = 0; i < props.getProperties().size(); i++)
	{
		EXPECT_EQ(props.getProperties()[i]->get<int>(), props2.getProperties()[i]->get<int>());
	}
}
