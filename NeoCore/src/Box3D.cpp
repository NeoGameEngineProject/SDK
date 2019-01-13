#include "Box3D.h"

#include <algorithm>

using namespace Neo;

void Box3D::makeAxisAligned()
{
	const Vector3 minTmp = Vector3(std::min(min.x, max.x), std::min(min.y, max.y), std::min(min.z, max.z));
	const Vector3 maxTmp = Vector3(std::max(min.x, max.x), std::max(min.y, max.y), std::max(min.z, max.z));
	
	min = minTmp;
	max = maxTmp;
}

void Box3D::addPoint(const Vector3& p)
{
	min = Vector3(
		std::min(min.x, p.x),
		std::min(min.y, p.y),
		std::min(min.z, p.z)
	);
	
	max = Vector3(
		std::max(max.x, p.x),
		std::max(max.y, p.y),
		std::max(max.z, p.z)
	);
}
