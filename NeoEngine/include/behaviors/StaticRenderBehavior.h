#ifndef NEO_STATICRENDERBEHAVIOR_H
#define NEO_STATICRENDERBEHAVIOR_H

#include <Behavior.h>

namespace Neo 
{

class StaticRenderBehavior : public Behavior
{
public:
	const char* getName() const override { return "StaticRender"; }
};

}

#endif // NEO_STATICRENDERBEHAVIOR_H
