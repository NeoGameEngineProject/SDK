#include "behaviors/MeshBehavior.h"
#include <cassert>

using namespace Neo;

void MeshBehavior::updateBoundingBox()
{
	// Find min and max
	if(m_submeshes.size() == 0)
		return;
	
	m_boundingBox = m_submeshes[0]->calculateBoundingBox();
	for(size_t i = 1; i < m_submeshes.size(); i++)
	{
		auto box = m_submeshes[i]->calculateBoundingBox();
		m_boundingBox.min = Vector3(MIN(box.min.x, m_boundingBox.min.x),
						MIN(box.min.y, m_boundingBox.min.y),
						MIN(box.min.z, m_boundingBox.min.z));
		
		m_boundingBox.max = Vector3(MAX(box.max.x, m_boundingBox.max.x),
						MAX(box.max.y, m_boundingBox.max.y),
						MAX(box.max.z, m_boundingBox.max.z));
	}
	
	m_boundingBox.diameter = (m_boundingBox.max - m_boundingBox.min).getLength();
}

MeshBehavior& MeshBehavior::operator= (const MeshBehavior& b)
{
	if(this != &b)
	{
		m_submeshes = b.m_submeshes;
	}
	
	return *this;
}
