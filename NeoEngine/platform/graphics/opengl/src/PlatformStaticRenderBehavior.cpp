#include "PlatformStaticRenderBehavior.h"
#include <behaviors/MeshBehavior.h>
#include <Object.h>
#include <Texture.h>
#include <Log.h>
#include <GL/glew.h>

using namespace Neo;

void PlatformStaticRenderBehavior::begin(Neo::Platform& p, Neo::Renderer& render, Level&)
{
	PlatformRenderer* prender = reinterpret_cast<PlatformRenderer*>(&render);
	assert(prender && "No renderer given!");
	
	m_mesh = getParent()->getBehavior<MeshBehavior>();

	auto& subMeshes = m_mesh->getMeshes();
	size_t bufferCount = subMeshes.size();

	m_vertexBuffers.alloc(bufferCount);
	m_indexBuffers.alloc(bufferCount);
	m_texcoordBuffers.alloc(bufferCount);
	m_vaos.alloc(bufferCount);

	glGenVertexArrays(bufferCount, m_vaos.data);
	glGenBuffers(bufferCount, m_vertexBuffers.data);
	glGenBuffers(bufferCount, m_indexBuffers.data);

	for(size_t i = 0; i < subMeshes.size(); i++)
	{
		unsigned int currentAttrib = 0;
		auto& submesh = subMeshes[i];

		glBindVertexArray(m_vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER,
					submesh->getVertices().size()*sizeof(MeshVertex),
					submesh->getVertices().data(),
					GL_STATIC_DRAW);

		glEnableVertexAttribArray(currentAttrib);
		glVertexAttribPointer(currentAttrib++, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), nullptr); // position

		glEnableVertexAttribArray(currentAttrib);
		glVertexAttribPointer(currentAttrib++, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(3*sizeof(float))); // normal

		glBindBuffer(GL_ARRAY_BUFFER, m_indexBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER,
					 submesh->getIndices().size()*sizeof(unsigned int),
					 submesh->getIndices().data(),
					 GL_STATIC_DRAW);

		auto numChannels = submesh->getTextureChannels().size();
		m_texcoordBuffers[i].alloc(numChannels);
		glGenBuffers(numChannels, m_texcoordBuffers[i].data);

		for(size_t j = 0; j < numChannels; j++)
		{
			auto& channel = submesh->getTextureChannels()[j];

			glBindBuffer(GL_ARRAY_BUFFER, m_texcoordBuffers[i][j]);
			glBufferData(GL_ARRAY_BUFFER,
						 channel.count * sizeof(Vector2),
						 channel.data,
						 GL_STATIC_DRAW);

			glEnableVertexAttribArray(currentAttrib);
			glVertexAttribPointer(currentAttrib++, 2, GL_FLOAT, GL_FALSE, 0, nullptr); // texcoord

			auto& material = submesh->getMaterial();
			auto texture = material.textures[j];
			assert(texture);

			if(texture->getID() == -1)
				texture->setID(prender->createTexture(texture));
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
}

void PlatformStaticRenderBehavior::end()
{
	
}

void PlatformStaticRenderBehavior::draw(Neo::Renderer& render)
{
	PlatformRenderer* prender = reinterpret_cast<PlatformRenderer*>(&render);

	prender->updateLights(m_mesh);
	prender->useShader(0);
	prender->setTransform(getParent()->getTransform());

	for(size_t i = 0; i < m_vaos.count; i++)
	{
		auto& submesh = m_mesh->getMeshes()[i];
		prender->setMaterial(submesh);

		glBindVertexArray(m_vaos[i]);
		glDrawArrays(GL_TRIANGLES, 0, submesh->getIndices().size());
	}

	glBindVertexArray(0);
}
