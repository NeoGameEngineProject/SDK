#ifndef NEO_BOX3D_H
#define NEO_BOX3D_H

#include "Vector3.h"

namespace Neo 
{

struct Box3D
{
	Vector3 min, max;
	
	void makeAxisAligned();
	void addPoint(const Vector3& p);
	float getDiameter() const { return (max - min).getLength(); }
};

}

#endif // NEO_BOX3D_H
