#ifndef COMMON_H
#define COMMON_H

#include <Renderer.h>
#include <bgfx/bgfx.h>
#include <vector>

namespace Neo
{
class Texture;
class LightBehavior;
class Level;
}

namespace BGFX
{

class Common : public Neo::Renderer
{
	struct BGFXVertexStruct
	{
		BGFXVertexStruct()
		{
			vertexStruct
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
				.end();
		}
		
		bgfx::VertexDecl vertexStruct;
	} m_bgfxVertStruct;
	
	struct BGFXTexCoordStruct
	{
		BGFXTexCoordStruct(unsigned int num)
		{
			auto mode = bgfx::Attrib::TexCoord0 + num;
			vertexStruct
				.begin()
				.add((bgfx::Attrib::Enum) mode, 2, bgfx::AttribType::Float)
				.end();
		}
		
		bgfx::VertexDecl vertexStruct;
	};

	BGFXTexCoordStruct m_texCoordStructs[4] = {
		BGFXTexCoordStruct(0),
		BGFXTexCoordStruct(1),
		BGFXTexCoordStruct(2),
		BGFXTexCoordStruct(3)
	};
	
	std::vector<bgfx::ProgramHandle> m_shaders;
	std::vector<bgfx::TextureHandle> m_textures;
	
public:
	unsigned int loadShader(const char* path);
	bgfx::VertexDecl getVertexStruct() const { return m_bgfxVertStruct.vertexStruct; }
	bgfx::VertexDecl getTexCoordStruct() const { return m_texCoordStructs[0].vertexStruct; }

	bgfx::ProgramHandle getShader(size_t idx) 
	{
		return m_shaders[idx];
	}
	
	bgfx::TextureHandle getTexture(size_t id) { return m_textures[id]; }
	size_t createTexture(Neo::Texture* tex);
};
}

#endif
