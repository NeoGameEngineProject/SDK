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

void Behavior::registerBehavior(std::unique_ptr<Behavior>&& behavior)
{
	LOG_INFO("Registering behavior:\t" << behavior->getName());
	Behavior* target = findBehaviorInRegistry(behavior->getName());
	if(target)
	{
		LOG_WARNING("Behavior " << behavior->getName() << " is already registered!");
		return;
	}
	
	s_registry.push_back(std::move(behavior));
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
