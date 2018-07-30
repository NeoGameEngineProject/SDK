#ifndef NEO_MESHBEHAVIOR_H
#define NEO_MESHBEHAVIOR_H

#include "Behavior.h"
#include <Mesh.h>

namespace Neo 
{

class MeshBehavior : public Neo::Behavior
{
	std::vector<MeshHandle> m_submeshes;
	AABB m_boundingBox;
	
public:
	MeshBehavior() {}
	MeshBehavior(MeshBehavior&& b) { *this = std::move(b); }
		
	MeshBehavior(MeshBehavior& b):
		m_submeshes(b.m_submeshes) {}
	
	const char* getName() const override { return "Mesh"; }
	Behavior* getNew() const override { return new MeshBehavior; }

	std::vector<MeshHandle>& getMeshes() { return m_submeshes; }
	
	MeshBehavior& operator= (MeshBehavior&& b)
	{
		if(this != &b)
		{
			m_submeshes = std::move(b.m_submeshes);
		}
		
		return *this;
	}
	
	MeshBehavior& operator= (const MeshBehavior& b);
	
	void updateBoundingBox();
	AABB getBoundingBox() const { return m_boundingBox; }
};

}

#endif // NEO_MESHBEHAVIOR_H
