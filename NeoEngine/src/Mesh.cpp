#include <Mesh.h>
#include <cassert>
#include <Maths.h>

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
