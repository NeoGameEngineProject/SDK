#ifndef NEO_LIGHTBEHAVIOR_H
#define NEO_LIGHTBEHAVIOR_H

#include <Behavior.h>

namespace Neo
{

class LightBehavior : public Neo::Behavior
{
public:
    const char* getName() const override { return "Sound"; }
};

}

#endif // NEO_LIGHTBEHAVIOR_H
