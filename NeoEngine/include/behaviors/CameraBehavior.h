#ifndef NEO_CAMERABEHAVIOR_H
#define NEO_CAMERABEHAVIOR_H

#include <Behavior.h>
#include <Matrix4x4.h>

namespace Neo
{

class CameraBehavior : public Neo::Behavior
{
public:
    const char* getName() const override { return "Camera"; }
    
    Matrix4x4 projectionMatrix;
    float fov = 60.0f;
    float near = 0.1f;
    float far = 1000.0f;
};

}

#endif // NEO_CAMERABEHAVIOR_H
