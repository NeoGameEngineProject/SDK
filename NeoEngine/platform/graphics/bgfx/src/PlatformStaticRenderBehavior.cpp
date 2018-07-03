#include "PlatformStaticRenderBehavior.h"
#include <behaviors/MeshBehavior.h>
#include <Object.h>
#include <Texture.h>

using namespace Neo;

void PlatformStaticRenderBehavior::begin(const Neo::Platform& p, Neo::Renderer& render)
{
	BGFXRenderer* bgfxRender = reinterpret_cast<BGFXRenderer*>(&render);
	m_mesh = getParent()->getBehavior<MeshBehavior>();
	auto& subMeshes = m_mesh->getSubMeshes();
	size_t bufferCount = subMeshes.size();
	
	m_vertexBuffers.alloc(bufferCount);
	m_indexBuffers.alloc(bufferCount);
	m_texcoordBuffers.alloc(bufferCount);

	for(size_t i = 0; i < subMeshes.size(); i++)
	{
		auto& k = subMeshes[i];
		
		const auto vertexRef = bgfx::makeRef(k.getVertices().data(), k.getVertices().size() * sizeof(MeshVertex));
		const auto indexRef = bgfx::makeRef(k.getIndices().data(), k.getIndices().size() * sizeof(unsigned int));
		
		m_vertexBuffers[i] = bgfx::createVertexBuffer(vertexRef, bgfxRender->getVertexStruct());
		m_indexBuffers[i] = bgfx::createIndexBuffer(indexRef, BGFX_BUFFER_INDEX32);
		
		m_texcoordBuffers[i].alloc(k.getTextureChannels().size());
		for(size_t j = 0; j < k.getTextureChannels().size(); j++)
		{
			auto& channel = k.getTextureChannels()[j];
			const auto texRef = bgfx::makeRef(channel.data, channel.count * sizeof(Vector2));
			m_texcoordBuffers[i][j] = bgfx::createVertexBuffer(texRef, bgfxRender->getTexCoordStruct());
			
			auto& material = k.getMaterial();
			auto texture = material.textures[j];
			assert(texture);
			
			if(texture->getID() == -1)
				texture->setID(bgfxRender->createTexture(texture));
		}
	}
	
	m_uMaterialDiffuse = bgfx::createUniform("diffuse", bgfx::UniformType::Vec4);
	m_uDiffuseTexture = bgfx::createUniform("diffuseTexture", bgfx::UniformType::Int1);
	m_uTextureConfig = bgfx::createUniform("textureConfig", bgfx::UniformType::Vec4);
	
	m_uMaterialSpecular = bgfx::createUniform("specular", bgfx::UniformType::Vec4);
	m_uMaterialEmit = bgfx::createUniform("emit", bgfx::UniformType::Vec4);
}

void PlatformStaticRenderBehavior::end()
{
	
}

#include <iostream>
void PlatformStaticRenderBehavior::draw(Neo::Renderer& render)
{
	BGFXRenderer* bgfxRender = reinterpret_cast<BGFXRenderer*>(&render);
	auto& subMeshes = m_mesh->getSubMeshes();
	
	bgfx::setTransform(getParent()->getTransform().entries);
	for(size_t i = 0; i < m_vertexBuffers.count; i++)
	{
		auto& material = subMeshes[i].getMaterial();
		
		Vector4 tmp;
		tmp = Vector4(material.diffuse);
		bgfx::setUniform(m_uMaterialDiffuse, &tmp);
		
		tmp = Vector4(material.specular);
		bgfx::setUniform(m_uMaterialSpecular, &tmp);
		
		tmp = Vector4(material.emit);
		bgfx::setUniform(m_uMaterialEmit, &tmp);
		
		tmp.x = subMeshes[i].getTextureChannels().size();
		tmp.y = material.shininess;
		tmp.z = material.opacity;

		bgfx::setUniform(m_uTextureConfig, &tmp);

		bgfx::setVertexBuffer(0, m_vertexBuffers[i]);
		
		if(subMeshes[i].getTextureChannels().size())
		{
			auto texture = material.textures[0];
			assert(texture);
						
			bgfx::setVertexBuffer(1, m_texcoordBuffers[i][0]);
			bgfx::setTexture(0, m_uDiffuseTexture, bgfxRender->getTexture(texture->getID()));
		}
				
		bgfx::setIndexBuffer(m_indexBuffers[i]);
		bgfx::submit(0, bgfxRender->getShader(0));
	}
}
