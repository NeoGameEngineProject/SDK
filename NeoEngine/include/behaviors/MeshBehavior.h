#ifndef NEO_MESHBEHAVIOR_H
#define NEO_MESHBEHAVIOR_H

#include "Behavior.h"

#include <vector>
#include <Vector3.h>
#include <Vector2.h>

namespace Neo 
{
class MeshBehavior : public Neo::Behavior
{
	std::vector<Vector3> m_vertices, m_normals;
	std::vector<Vector2> m_texcoords;
	
	std::vector<std::vector<unsigned int>> m_submeshes;
	
public:
	MeshBehavior() {}
	MeshBehavior(MeshBehavior&& b): 
		m_vertices(std::move(b.m_vertices)),
		m_normals(std::move(b.m_normals)),
		m_texcoords(std::move(b.m_texcoords)),
		m_submeshes(std::move(b.m_submeshes)) {}
	
	const char* getName() const override { return "Mesh"; }

	std::vector<Vector3>& getVertices() { return m_vertices; }
	std::vector<Vector3>& getNormals() { return m_normals; }
	std::vector<Vector2>& getTexCoords() { return m_texcoords; }
	std::vector<std::vector<unsigned int>>& getSubmeshes() { return m_submeshes; }
	
	MeshBehavior& operator= (MeshBehavior&& b)
	{
		if(this != &b)
		{
			m_vertices = std::move(b.m_vertices);
			m_normals = std::move(b.m_normals);
			m_texcoords = std::move(b.m_texcoords);
			m_submeshes = std::move(b.m_submeshes);
		}
		
		return *this;
	}
};

}

#endif // NEO_MESHBEHAVIOR_H
