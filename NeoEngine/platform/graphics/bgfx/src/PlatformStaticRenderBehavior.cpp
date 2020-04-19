#include "PlatformStaticRenderBehavior.h"
#include <behaviors/MeshBehavior.h>
#include <Object.h>
#include <Texture.h>

using namespace Neo;

void PlatformStaticRenderBehavior::begin(Neo::Platform& p, Neo::Renderer& render, Level& level)
{
	PlatformRenderer* bgfxRender = reinterpret_cast<PlatformRenderer*>(&render);
	m_mesh = getParent()->getBehavior<MeshBehavior>();
	auto& subMeshes = m_mesh->getMeshes();
	size_t bufferCount = subMeshes.size();
	
	m_vertexBuffers.alloc(bufferCount);
	m_indexBuffers.alloc(bufferCount);
	m_texcoordBuffers.alloc(bufferCount);

	for(size_t i = 0; i < subMeshes.size(); i++)
	{
		auto& k = subMeshes[i];
		
		const auto vertexRef = bgfx::makeRef(k->getVertices().data(), k->getVertices().size() * sizeof(MeshVertex));
		const auto indexRef = bgfx::makeRef(k->getIndices().data(), k->getIndices().size() * sizeof(unsigned int));
		
		m_vertexBuffers[i] = bgfx::createVertexBuffer(vertexRef, bgfxRender->getVertexStruct());
		m_indexBuffers[i] = bgfx::createIndexBuffer(indexRef, BGFX_BUFFER_INDEX32);
		
		m_texcoordBuffers[i].alloc(k->getTextureChannels().size());
		for(size_t j = 0; j < k->getTextureChannels().size(); j++)
		{
			auto& channel = k->getTextureChannels()[j];
			const auto texRef = bgfx::makeRef(channel.data, channel.count * sizeof(Vector2));
			m_texcoordBuffers[i][j] = bgfx::createVertexBuffer(texRef, bgfxRender->getTexCoordStruct());
			
			auto& material = k->getMaterial();
			auto texture = material.textures[j];
			assert(texture);
			
			if(texture->getID() == -1)
				texture->setID(bgfxRender->createTexture(texture));
		}
	}
	
	m_uMaterialDiffuse = bgfx::createUniform("u_diffuse", bgfx::UniformType::Vec4);
	m_uMaterialSpecular = bgfx::createUniform("u_specular", bgfx::UniformType::Vec4);
	m_uMaterialEmit = bgfx::createUniform("u_emit", bgfx::UniformType::Vec4);
	
	m_uDiffuseTexture = bgfx::createUniform("diffuseTexture", bgfx::UniformType::Sampler);
	m_uTextureConfig = bgfx::createUniform("textureConfig", bgfx::UniformType::Vec4);
}

void PlatformStaticRenderBehavior::end()
{
	
}

void PlatformStaticRenderBehavior::draw(Neo::Renderer& render)
{
	PlatformRenderer* bgfxRender = reinterpret_cast<PlatformRenderer*>(&render);
	auto& subMeshes = m_mesh->getMeshes();
	
	bgfxRender->updateLights(m_mesh);
	bgfx::setTransform(getParent()->getTransform().entries);
	for(size_t i = 0; i < m_vertexBuffers.count; i++)
	{
		auto& material = subMeshes[i]->getMaterial();
		
		Vector4 tmp;
		tmp = Vector4(material.diffuseColor);
		tmp.w = material.opacity;
		bgfx::setUniform(m_uMaterialDiffuse, &tmp);
		
		tmp = Vector4(material.specularColor);
		bgfx::setUniform(m_uMaterialSpecular, &tmp);
		
		tmp = Vector4(material.emitColor);
		bgfx::setUniform(m_uMaterialEmit, &tmp);
		
		tmp.x = subMeshes[i]->getTextureChannels().size();
		tmp.y = material.shininess;
		tmp.z = material.opacity;

		bgfx::setUniform(m_uTextureConfig, &tmp);
		bgfx::setVertexBuffer(0, m_vertexBuffers[i]);
		
		if(subMeshes[i]->getTextureChannels().size())
		{
			auto texture = material.textures[0];
			assert(texture);
						
			bgfx::setVertexBuffer(1, m_texcoordBuffers[i][0]);
			bgfx::setTexture(0, m_uDiffuseTexture, bgfxRender->getTexture(texture->getID()));
		}
		
		uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW;
		
		switch(material.blendMode)
		{
			case BLENDING_ALPHA: state |= BGFX_STATE_BLEND_ALPHA; break;
			case BLENDING_ADD: state |= BGFX_STATE_BLEND_ADD; break;
			case BLENDING_SUB: state |= BGFX_STATE_BLEND_DARKEN; break;
			case BLENDING_LIGHT: state |= BGFX_STATE_BLEND_LIGHTEN; break;
			case BLENDING_PRODUCT: state |= BGFX_STATE_BLEND_MULTIPLY; break;
			
			default:
			case BLENDING_NONE: break;
		}
		
		int depth = 1;
		if(material.opacity == 1.0f)
		{
			depth = 0;
			state |= BGFX_STATE_WRITE_Z;
		}
		
		bgfx::setState(state);
		
		bgfx::setIndexBuffer(m_indexBuffers[i]);
		bgfx::submit(0, bgfxRender->getShader(0), depth);
	}
}
