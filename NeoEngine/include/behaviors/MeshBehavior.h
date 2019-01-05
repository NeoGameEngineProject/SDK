#ifndef NEO_MESHBEHAVIOR_H
#define NEO_MESHBEHAVIOR_H

#include "Behavior.h"
#include <Mesh.h>

namespace Neo 
{
class Level;
class MeshBehavior : public Neo::Behavior
{
	std::vector<MeshHandle> m_submeshes;
	Box3D m_boundingBox;
	
public:
	MeshBehavior() {}
	MeshBehavior(MeshBehavior&& b) { *this = std::move(b); }
		
	MeshBehavior(MeshBehavior& b):
		m_submeshes(b.m_submeshes) {}
	
	bool isEditorVisible() const override { return false; }
	
	void copyTo(Behavior& b) const override;
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
	Box3D getBoundingBox() const { return m_boundingBox; }

	void serialize(std::ostream& out) override;
	void deserialize(Level& level, std::istream& out) override;
};

}

#endif // NEO_MESHBEHAVIOR_H
