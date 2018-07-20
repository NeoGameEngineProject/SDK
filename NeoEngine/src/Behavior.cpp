#include "Behavior.h"

using namespace Neo;

std::unique_ptr<Behavior> Behavior::clone() const
{
	Behavior* newBehavior = getNew();
	*newBehavior = *this;
	
	return std::unique_ptr<Behavior>(newBehavior);
}
