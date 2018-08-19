#include <Mesh.h>
#include <cassert>
#include <Maths.h>
#include <Level.h>

#include <ostream>
#include <istream>

using namespace Neo;

void Mesh::set(size_t numVertices, 
			Vector3* vertices,
			Vector3* normals, 
			size_t numIndices, 
			unsigned int* indices)
{
	assert(!numVertices || numVertices && vertices);
	assert(!numIndices || numIndices && indices);
	
	if(numVertices)
	{
		m_meshVertices.resize(numVertices);
		for(size_t i = 0; i < numVertices; i++)
		{
			m_meshVertices.data()[i] = { vertices[i], normals[i] };
		}
	}
	
	if(numIndices)
	{
		m_indices.resize(numIndices);
		memcpy(m_indices.data(), indices, numIndices*sizeof(unsigned int));
	}
}

AABB Mesh::calculateBoundingBox()
{
	AABB box;
	for(size_t i = 0; i < m_meshVertices.size(); i++)
	{
		const auto& vertex = m_meshVertices[i];
		box.min = Vector3(MIN(vertex.position.x, box.min.x),
				  MIN(vertex.position.y, box.min.y),
				  MIN(vertex.position.z, box.min.z));
		
		box.max = Vector3(MAX(vertex.position.x, box.max.x),
				  MAX(vertex.position.y, box.max.y),
				  MAX(vertex.position.z, box.max.z));
	}
	
	box.diameter = (box.max - box.min).getLength();
	return box;
}

void Mesh::serialize(std::ostream& out)
{
	m_name.serialize(out);
	
	uint32_t value = 0;
	value = getIndices().size();
	out.write((char*) &value, sizeof(value));
	out.write((char*) getIndices().data(), sizeof(unsigned int) * value);

	value = getVertices().size();
	out.write((char*) &value, sizeof(value));
	out.write((char*) getVertices().data(), sizeof(MeshVertex) * value);

	value = getTextureChannels().size();
	out.write((char*) &value, sizeof(value));

	for(auto& channel : getTextureChannels())
	{
		value = channel.count;
		out.write((char*) &value, sizeof(value));
		out.write((char*) channel.data, sizeof(Vector2) * value);
	}

	auto& material = getMaterial();
	out.write((char*) &material.diffuseColor, sizeof(Vector3));
	out.write((char*) &material.specularColor, sizeof(Vector3));
	out.write((char*) &material.emitColor, sizeof(Vector3));
	out.write((char*) &material.customColor, sizeof(Vector3));
	out.write((char*) &material.shininess, sizeof(float));
	out.write((char*) &material.opacity, sizeof(float));
	out.write((char*) &material.customValue, sizeof(float));

	// FIXME: NEED TO DEFINE BLEND MODE VALUES!
	out.write((char*) &material.blendMode, sizeof(BLENDING_MODES));

	FixedString<1> empty;
	for(unsigned short i = 0; i < 4; i++)
	{
		if(material.textures[i] == nullptr)
			empty.serialize(out);
		else
			material.textures[i]->getPath().serialize(out);
	}
}

void Mesh::deserialize(Level& level, std::istream& in)
{
	m_name.deserialize(in);
	
	uint32_t value = 0;
	in.read((char*) &value, sizeof(value));
	getIndices().resize(value);
	in.read((char*) getIndices().data(), sizeof(unsigned int) * value);

	in.read((char*) &value, sizeof(value));
	getVertices().resize(value);
	in.read((char*) getVertices().data(), sizeof(MeshVertex) * value);

	in.read((char*) &value, sizeof(value));
	getTextureChannels().resize(value);
	
	for(auto& channel : getTextureChannels())
	{
		in.read((char*) &value, sizeof(value));
		channel.alloc(value);
		
		in.read((char*) channel.data, sizeof(Vector2) * value);
	}

	Material material;
	in.read((char*) &material.diffuseColor, sizeof(Vector3));
	in.read((char*) &material.specularColor, sizeof(Vector3));
	in.read((char*) &material.emitColor, sizeof(Vector3));
	in.read((char*) &material.customColor, sizeof(Vector3));
	in.read((char*) &material.shininess, sizeof(float));
	in.read((char*) &material.opacity, sizeof(float));
	in.read((char*) &material.customValue, sizeof(float));

	// FIXME: NEED TO DEFINE BLEND MODE VALUES!
	in.read((char*) &material.blendMode, sizeof(BLENDING_MODES));

	for(unsigned short i = 0; i < 4; i++)
	{
		FixedString<128> path;
		path.deserialize(in);
		
		if(path.getLength())
			material.textures[i] = level.loadTexture(path.str());
	}
	
	setMaterial(material);
}

