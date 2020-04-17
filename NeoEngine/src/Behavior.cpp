#include "Behavior.h"
#include <Log.h>

using namespace Neo;

std::vector<std::unique_ptr<Behavior>>* getRegistry()
{
	static std::vector<std::unique_ptr<Behavior>> s_registry;
	return &s_registry;
}

Behavior* Behavior::findBehaviorInRegistry(const char* name)
{
	Behavior* target = nullptr;
	for(auto& b : *getRegistry())
		if(!strcmp(b->getName(), name))
		{
			target = b.get();
			break;
		}
	return target;
}

std::unique_ptr<Behavior> Behavior::create(const char* name)
{
	LOG_DEBUG("Behavior registry at: " << getRegistry());
	
	auto target = findBehaviorInRegistry(name);
	if(!target)
	{
		LOG_ERROR("Could not find behavior with the name \"" << name << "\" in registry!");
		return nullptr;
	}
	
	return std::unique_ptr<Behavior>(target->getNew());
}

unsigned int Behavior::registerBehavior(std::unique_ptr<Behavior>&& behavior)
{
	LOG_DEBUG("Behavior registry at: " << getRegistry());

	Behavior* target = findBehaviorInRegistry(behavior->getName());
	if(target)
	{
		LOG_WARNING("Behavior " << behavior->getName() << " is already registered!");
		return -1;
	}
	
	LOG_INFO("Registering behavior:\t" << behavior->getName());
	getRegistry()->push_back(std::move(behavior));
	return getRegistry()->size() - 1;
}

void Behavior::unregisterBehavior(const char* name)
{
	LOG_DEBUG("Behavior registry at: " << getRegistry());
	
	for(auto& b : *getRegistry())
	{
		if(!strcmp(b->getName(), name))
		{
			LOG_INFO("Unregistering behavior:\t" << name);
			getRegistry()->erase(getRegistry()->begin());
		}
	}
}

void Behavior::unregisterBehavior(unsigned int index)
{
	LOG_DEBUG("Behavior registry at: " << getRegistry());
	
	// Invalid entry, e.g. the register failed
	if(index == -1)
		return;
	
	assert(index < getRegistry()->size());
	getRegistry()->erase(getRegistry()->begin() + index);
}

const std::vector<std::unique_ptr<Behavior>>& Behavior::registeredBehaviors()
{
	return *getRegistry();
}

std::unique_ptr<Behavior> Behavior::clone() const
{
	Behavior* newBehavior = getNew();
	*newBehavior = *this;
	
	return std::unique_ptr<Behavior>(newBehavior);
}

Behavior& Behavior::operator=(const Behavior& b)
{
	b.copyTo(*this);
	return *this;
}
