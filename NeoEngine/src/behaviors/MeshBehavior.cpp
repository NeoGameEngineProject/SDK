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
