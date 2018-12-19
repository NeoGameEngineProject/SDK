#include "Behavior.h"
#include <Log.h>

using namespace Neo;

std::vector<std::unique_ptr<Behavior>> Behavior::s_registry;

Behavior* Behavior::findBehaviorInRegistry(const char* name)
{
	Behavior* target = nullptr;
	for(auto& b : Behavior::s_registry)
		if(!strcmp(b->getName(), name))
		{
			target = b.get();
			break;
		}
	return target;
}

std::unique_ptr<Behavior> Behavior::create(const char* name)
{
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
	Behavior* target = findBehaviorInRegistry(behavior->getName());
	if(target)
	{
		LOG_WARNING("Behavior " << behavior->getName() << " is already registered!");
		return -1;
	}
	
	LOG_INFO("Registering behavior:\t" << behavior->getName());
	s_registry.push_back(std::move(behavior));
	return s_registry.size() - 1;
}

void Behavior::unregisterBehavior(const char* name)
{
	for(auto& b : s_registry)
	{
		if(!strcmp(b->getName(), name))
		{
			LOG_INFO("Unregistering behavior:\t" << name);
			s_registry.erase(s_registry.begin());
		}
	}
}

void Behavior::unregisterBehavior(unsigned int index)
{
	// Invalid entry, e.g. the register failed
	if(index == -1)
		return;
	
	assert(index < s_registry.size());
	s_registry.erase(s_registry.begin() + index);
}

const std::vector<std::unique_ptr<Behavior>>& Behavior::registeredBehaviors()
{
	return s_registry;
}

std::unique_ptr<Behavior> Behavior::clone() const
{
	Behavior* newBehavior = getNew();
	*newBehavior = *this;
	
	return std::unique_ptr<Behavior>(newBehavior);
}
