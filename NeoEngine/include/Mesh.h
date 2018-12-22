#ifndef MESH_H
#define MESH_H

#include <vector>
#include <Vector3.h>
#include <Vector2.h>
#include <Image.h>
#include <Array.h>
#include <Handle.h>
#include <FixedString.h>

#include <iosfwd>

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
	Vector3 diffuseColor = Vector3(1, 1, 1);
	Vector3 specularColor = Vector3(1, 1, 1);
	Vector3 emitColor;
	Vector3 customColor;
	
	enum TEXTURE_TYPE
	{
		DIFFUSE = 0,
		NORMAL = 1,
		SPECULAR = 2,
		HEIGHT = 3,
		
		TEXTURE_MAX = 4
	};
	
	Texture* textures[8] {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	BLENDING_MODES blendMode = BLENDING_ALPHA;
};

struct AABB
{
	Vector3 min, max;
	float diameter;
};

struct MeshVertex
{
	Vector3 position, normal;
};

struct Tangent
{
	Vector3 tangent, bitangent;
};

class Level;
class Mesh
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
	
	const Material& getMaterial() const { return m_material; }
	void setMaterial(Material& material) { m_material = material; }
	
	AABB calculateBoundingBox();
	
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
