#ifndef NEO_MESHBEHAVIOR_H
#define NEO_MESHBEHAVIOR_H

#include "Behavior.h"

#include <vector>
#include <Vector3.h>
#include <Vector2.h>

namespace Neo 
{

struct MeshVertex
{
	Vector3 position, normal;
	Vector2 texcoord;
};

struct Material
{
	// TODO Blend mode
	float opacity;
	float shininess;
	float customValue;
	Vector3 diffuse;
	Vector3 specular;
	Vector3 emit;
	Vector3 customColor;
};

class SubMesh
{
public:
	SubMesh() {}
	SubMesh(SubMesh&& b):
		m_indices(std::move(b.m_indices)),
		m_meshVertices(std::move(b.m_meshVertices)),
		m_material(b.m_material) {}
	
	SubMesh(const SubMesh& b):
		m_indices(b.m_indices),
		m_meshVertices(b.m_meshVertices),
		m_material(b.m_material) {}
	
	std::vector<MeshVertex>& getVertices() { return m_meshVertices; }
	std::vector<unsigned int>& getIndices() { return m_indices; }
	
	void set(size_t numVertices,
		Vector3* vertices,
		Vector3* normals,
		Vector2* texcoords,
		size_t numIndices,
		unsigned int* indices);
	
	const Material& getMaterial() const { return m_material; }
	void setMaterial(Material& material) { m_material = material; }
	
private:
	std::vector<unsigned int> m_indices;
	std::vector<MeshVertex> m_meshVertices;
	Material m_material;
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
