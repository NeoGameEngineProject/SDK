#include "behaviors/MeshBehavior.h"
#include <cassert>

using namespace Neo;

void SubMesh:: set(size_t numVertices, 
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

AABB SubMesh::calculateBoundingBox()
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
	
	return box;
}

void MeshBehavior::updateBoundingBox()
{
	// Find min and max
	if(m_submeshes.size() == 0)
		return;
	
	m_boundingBox = m_submeshes[0].calculateBoundingBox();
	for(size_t i = 1; i < m_submeshes.size(); i++)
	{
		auto box = m_submeshes[i].calculateBoundingBox();
		m_boundingBox.min = Vector3(MIN(box.min.x, m_boundingBox.min.x),
						MIN(box.min.y, m_boundingBox.min.y),
						MIN(box.min.z, m_boundingBox.min.z));
		
		m_boundingBox.max = Vector3(MAX(box.max.x, m_boundingBox.max.x),
						MAX(box.max.y, m_boundingBox.max.y),
						MAX(box.max.z, m_boundingBox.max.z));
	}
}
