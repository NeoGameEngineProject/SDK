#include "behaviors/MeshBehavior.h"
#include <iostream>

using namespace Neo;

void SubMesh:: set(size_t numVertices, 
			Vector3* vertices,
			Vector3* normals, 
			Vector2* texcoords, 
			size_t numIndices, 
			unsigned int* indices)
{
	m_meshVertices.resize(numVertices);
	for(size_t i = 0; i < numVertices; i++)
	{
		if(texcoords)
			m_meshVertices.data()[i] = { vertices[i], normals[i], texcoords[i] };
		else
			m_meshVertices.data()[i] = { vertices[i], normals[i], Vector2(0, 0) };
	}
	
	m_indices.resize(numIndices);
	memcpy(m_indices.data(), indices, numIndices*sizeof(unsigned int));
}
