#include "behaviors/MeshBehavior.h"

#include <Level.h>
#include <Texture.h>

#include <cassert>
#include <ostream>
#include <istream>

using namespace Neo;

REGISTER_BEHAVIOR(MeshBehavior)

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
}

MeshBehavior& MeshBehavior::operator= (const MeshBehavior& b)
{
	if(this != &b)
	{
		m_submeshes = b.m_submeshes;
	}
	
	return *this;
}

void MeshBehavior::copyTo(Behavior& b) const
{
	assert(b.getName() == getName());
	reinterpret_cast<MeshBehavior*>(&b)->m_submeshes = m_submeshes;
}

void MeshBehavior::serialize(std::ostream& out)
{
	uint16_t smallValue = m_submeshes.size();
	out.write((char*) &smallValue, sizeof(uint16_t));

	for(auto& submesh : m_submeshes)
	{
		FixedString<128> str = submesh->getName();
		str.serialize(out);
	}
}

void MeshBehavior::deserialize(Level& level, std::istream& in)
{
	uint16_t smallValue;
	in.read((char*) &smallValue, sizeof(uint16_t));

	m_submeshes.reserve(smallValue);
	for(uint16_t i = 0; i < smallValue; i++)
	{
		FixedString<128> name;
		name.deserialize(in);
		m_submeshes.push_back(level.loadMesh(name.str()));
	}
}
