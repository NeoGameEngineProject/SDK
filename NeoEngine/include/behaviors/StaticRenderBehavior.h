#ifndef NEO_STATICRENDERBEHAVIOR_H
#define NEO_STATICRENDERBEHAVIOR_H

#include <Behavior.h>
#include <PlatformStaticRenderBehavior.h>

namespace Neo 
{

class StaticRenderBehavior : public PlatformStaticRenderBehavior
{
public:
	const char* getName() const override { return "StaticRender"; }
	Behavior* getNew() const override;
};

}

#endif // NEO_STATICRENDERBEHAVIOR_H
