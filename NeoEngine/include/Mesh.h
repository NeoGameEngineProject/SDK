#ifndef MESH_H
#define MESH_H

#include <vector>
#include <Vector3.h>
#include <Vector2.h>
#include <Image.h>
#include <Array.h>
#include <Handle.h>
#include <FixedString.h>
#include <Box3D.h>
#include <Material.h>
#include "NeoEngine.h"

#include <iosfwd>

namespace Neo
{

struct MeshVertex
{
	Vector3 position, normal;
};

struct Tangent
{
	Vector3 tangent, bitangent;
};

class Level;
class NEO_ENGINE_EXPORT Mesh
{
public:
	Mesh() {}
	Mesh(Mesh&& b):
		m_indices(std::move(b.m_indices)),
		m_meshVertices(std::move(b.m_meshVertices)),
		m_meshTangents(std::move(b.m_meshTangents)),
		m_textureChannels(std::move(b.m_textureChannels)),
		m_material(b.m_material),
		m_name(b.m_name){}
	
	Mesh(const Mesh& b)
	{
		*this = b;
	}
	
	Mesh& operator= (const Mesh& b)
	{
		m_indices = b.m_indices;
		m_meshVertices = b.m_meshVertices;
		m_textureChannels = b.m_textureChannels;
		m_material = b.m_material;
		m_meshTangents = b.m_meshTangents;
		m_name = b.m_name;
		return *this;
	}
	
	std::vector<MeshVertex>& getVertices() { return m_meshVertices; }
	std::vector<Tangent>& getTangents() { return m_meshTangents; }
	std::vector<unsigned int>& getIndices() { return m_indices; }
	std::vector<Array<Vector2>>& getTextureChannels() { return m_textureChannels; }
	
	void set(size_t numVertices,
		Vector3* vertices,
		Vector3* normals,
		Vector3* tangents,
		Vector3* bitangents,
		size_t numIndices,
		unsigned int* indices);
	
	Material& getMaterial() { return m_material; }
	const Material& getMaterial() const { return m_material; }
	void setMaterial(Material& material) { m_material = material; }
	
	Box3D calculateBoundingBox();
	
	void serialize(std::ostream& out);
	void deserialize(Level& level, std::istream& in);
	
	const char* getName() const { return m_name.str(); }
	void setName(const char* name) { m_name = name; }
	
private:
	std::vector<unsigned int> m_indices;
	std::vector<MeshVertex> m_meshVertices;
	std::vector<Tangent> m_meshTangents;
	
	std::vector<Array<Vector2>> m_textureChannels;
	
	Material m_material;
	FixedString<128> m_name;
};

typedef Handle<Mesh, std::vector<Mesh>> MeshHandle;
typedef Handle<Mesh, Array<Mesh>> MeshHandleArray;

}

#endif
