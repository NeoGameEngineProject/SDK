#include <Common.h>
#include <string>
#include <Texture.h>
#include <FileTools.h>

#include <iostream>

/*
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb/stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_RECT_PACK_VERSION

#include <stb/stb_truetype.h>
*/

using namespace BGFX;
using namespace Neo;

unsigned int Common::loadShader(const char* path)
{
	std::string fullpath = path;
	
	unsigned int vertShaderSize = 0;
	char* vertShaderData = readBinaryFile((fullpath + "_vs.bin").c_str(), &vertShaderSize);
	if(!vertShaderData)
	{
		std::cerr << "Error: Could not load shader " << path << "_vs.bin" << std::endl;
		return -1;
	}

	auto vertShader = bgfx::createShader(bgfx::copy(vertShaderData, vertShaderSize));
	
	unsigned int fragShaderSize = 0;
	char* fragShaderData = readBinaryFile((fullpath + "_fs.bin").c_str(), &fragShaderSize);

	if(!fragShaderData)
	{
		delete vertShaderData;
		std::cerr << "Error: Could not load shader " << path << "_fs.bin" << std::endl;
		return -1;
	}
	auto fragShader = bgfx::createShader(bgfx::copy(fragShaderData, fragShaderSize));
	
	auto program = bgfx::createProgram(vertShader, fragShader, true);
	m_shaders.push_back(program);
	
	delete vertShaderData;
	delete fragShaderData;
	
	return m_shaders.size() - 1;
}

size_t Common::createTexture(Texture* tex)
{
	const auto texRef = bgfx::makeRef(tex->getData(), tex->getStorageSize());
	auto format = bgfx::TextureFormat::RGBA8;
	if(tex->getComponents() == 3)
		format = bgfx::TextureFormat::RGB8;
	
	m_textures.push_back(bgfx::createTexture2D(tex->getWidth(), tex->getHeight(), tex->hasMipMap(), 1, format, 0, texRef));
	return m_textures.size() - 1;
}


