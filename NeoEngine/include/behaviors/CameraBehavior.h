#ifndef NEO_CAMERABEHAVIOR_H
#define NEO_CAMERABEHAVIOR_H

#include <Behavior.h>

namespace Neo
{

class CameraBehavior : public Neo::Behavior
{
public:
    const char* getName() const override { return "Camera"; }
};

}

#endif // NEO_CAMERABEHAVIOR_H
