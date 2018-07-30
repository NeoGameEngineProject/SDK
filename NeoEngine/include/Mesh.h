#ifndef MESH_H
#define MESH_H

#include <vector>
#include <Vector3.h>
#include <Vector2.h>
#include <Image.h>
#include <Array.h>

namespace Neo
{

enum BLENDING_MODES
{
	BLENDING_NONE = 0,
	BLENDING_ALPHA,
	BLENDING_ADD,
	BLENDING_SUB,
	BLENDING_LIGHT,
	BLENDING_PRODUCT
};

enum TEX_MODES
{
	TEX_DEPTH = 0,
	TEX_R = 1,
	TEX_RG = 2,
	TEX_RGB = 3,
	TEX_RGBA = 4
};

enum WRAP_MODES
{
	WRAP_REPEAT = 0,
	WRAP_CLAMP
};

enum CULL_MODES
{
	CULL_NONE = 0,
	CULL_FRONT,
	CULL_BACK,
	CULL_FRONT_BACK
};

class Texture;
struct Material
{
	// TODO Blend mode
	float opacity = 1;
	float shininess = 32;
	float customValue;
	Vector3 diffuse = Vector3(1, 1, 1);
	Vector3 specular = Vector3(1, 1, 1);
	Vector3 emit;
	Vector3 customColor;
	Texture* textures[4] {nullptr, nullptr, nullptr, nullptr};
	BLENDING_MODES blendMode = BLENDING_ALPHA;
};

struct AABB
{
	Vector3 min, max;
};

struct MeshVertex
{
	Vector3 position, normal;
};

class Mesh
{
public:
	Mesh() {}
	Mesh(Mesh&& b):
		m_indices(std::move(b.m_indices)),
		m_meshVertices(std::move(b.m_meshVertices)),
		m_textureChannels(std::move(b.m_textureChannels)),
		m_material(b.m_material) {}
	
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
		return *this;
	}
	
	std::vector<MeshVertex>& getVertices() { return m_meshVertices; }
	std::vector<unsigned int>& getIndices() { return m_indices; }
	std::vector<Array<Vector2>>& getTextureChannels() { return m_textureChannels; }
	
	void set(size_t numVertices,
		Vector3* vertices,
		Vector3* normals,
		size_t numIndices,
		unsigned int* indices);
	
	const Material& getMaterial() const { return m_material; }
	void setMaterial(Material& material) { m_material = material; }
	
	AABB calculateBoundingBox();
	
private:
	std::vector<unsigned int> m_indices;
	std::vector<MeshVertex> m_meshVertices;
	std::vector<Array<Vector2>> m_textureChannels;
	
	Material m_material;
};

}

#endif
