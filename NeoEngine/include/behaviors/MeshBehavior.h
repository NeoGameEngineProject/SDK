#ifndef NEO_MESHBEHAVIOR_H
#define NEO_MESHBEHAVIOR_H

#include "Behavior.h"

#include <vector>
#include <Vector3.h>
#include <Vector2.h>

namespace Neo 
{

class SubMesh
{
public:
	SubMesh() {}
	SubMesh(SubMesh&& b):
		m_vertices(std::move(b.m_vertices)),
		m_normals(std::move(b.m_normals)),
		m_texcoords(std::move(b.m_texcoords)),
		m_indices(std::move(b.m_indices)) {}
	
	SubMesh(const SubMesh& b):
		m_vertices(b.m_vertices),
		m_normals(b.m_normals),
		m_texcoords(b.m_texcoords),
		m_indices(b.m_indices){}
	
	std::vector<Vector3>& getVertices() { return m_vertices; }
	std::vector<Vector3>& getNormals() { return m_normals; }
	std::vector<Vector2>& getTexCoords() { return m_texcoords; }
	std::vector<unsigned int>& getIndices() { return m_indices; }
	
private:
	std::vector<Vector3> m_vertices, m_normals;
	std::vector<Vector2> m_texcoords;
	std::vector<unsigned int> m_indices;
};

class MeshBehavior : public Neo::Behavior
{
	std::vector<SubMesh> m_submeshes;
	
public:
	MeshBehavior() {}
	MeshBehavior(MeshBehavior&& b) { *this = std::move(b); }
		
	MeshBehavior(MeshBehavior& b):
		m_submeshes(b.m_submeshes) {}
	
	const char* getName() const override { return "Mesh"; }
	std::vector<SubMesh>& getSubMeshes() { return m_submeshes; }
	
	MeshBehavior& operator= (MeshBehavior&& b)
	{
		if(this != &b)
		{
			m_submeshes = std::move(b.m_submeshes);
		}
		
		return *this;
	}
};

}

#endif // NEO_MESHBEHAVIOR_H
