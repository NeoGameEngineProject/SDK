#include <Common.h>
#include <string>
#include <memory>

#include <Material.h>
#include <Texture.h>
#include <FileTools.h>
#include <Log.h>

#include <iostream>
#include <StringTools.h>

#include <GL/glew.h>

#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

#include <cassert>

#include <regex>
#include <unordered_map>

using namespace Neo;

namespace
{

bool checkShaderStatus(GLuint shader, const char* name)
{
	GLint result = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if(!result)
	{
		LOG_ERROR("Could not compile shader: " << name);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &result);
		if(result)
		{
			auto log = std::unique_ptr<char[]>(new char[result]);
			glGetShaderInfoLog(shader, result, nullptr, log.get());
			LOG_INFO(log.get());
		}
		return false;
	}

	return true;
}

bool checkProgramStatus(GLuint shader)
{
	GLint result = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if(!result)
	{
		LOG_ERROR("Could not link shader!");
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &result);
		if(result)
		{
			auto log = std::unique_ptr<char[]>(new char[result]);
			glGetProgramInfoLog(shader, result, nullptr, log.get());
			LOG_INFO(log.get());
		}
		return false;
	}

	return true;
}
}

Common::~Common()
{
	for(auto shader : m_shaders)
		glDeleteProgram(shader.id);
}

void Common::useShader(unsigned int id)
{
	assert(id < m_shaders.size());
	assert(m_shaders[id].id != -1);
	
	glUseProgram(m_shaders[id].id);
}

int Common::loadShader(const std::string& vert, const std::string& frag)
{
	return loadShader(vert.c_str(), frag.c_str());
}

int Common::loadShader(const char* vert, const char* frag)
{
	GLuint vertexShader = -1, fragmentShader = -1;

	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if(!vertexShader)
		{
			LOG_ERROR("Could not create OpenGL vertex shader!");
			return -1;
		}

		const char* array[] = {vert};
		glShaderSource(vertexShader, 1, array, nullptr);
		glCompileShader(vertexShader);

		if(!checkShaderStatus(vertexShader, vert))
		{
			glDeleteShader(vertexShader);
			return -1;
		}
	}

	{
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if(!fragmentShader)
		{
			LOG_ERROR("Could not create OpenGL fragment shader!");
			return -1;
		}

		const char* array[] = {frag};
		glShaderSource(fragmentShader, 1, array, nullptr);
		glCompileShader(fragmentShader);

		if(!checkShaderStatus(fragmentShader, frag))
		{
			glDeleteShader(fragmentShader);
			return -1;
		}
	}

	GLuint program = glCreateProgram();
	if(!program)
	{
		program = -1;
		LOG_ERROR("Could not create shader program!");
		goto done;
	}

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	if(!checkProgramStatus(program))
	{
		LOG_ERROR("Could not link shader!");
		glDeleteProgram(program);
		program = -1;
		goto done;
	}

done:
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	return program;
}

int Common::loadShader(const char* path)
{
	std::string fullpath = path;

	const auto vertShaderData = preprocess((fullpath + "_vs.glsl").c_str());
	if (vertShaderData.empty())
	{
		LOG_ERROR("Could not load shader " << path << "_vs.glsl");
		return -1;
	}

		
	const auto fragShaderData = preprocess((fullpath + "_fs.glsl").c_str());
	if(fragShaderData.empty())
	{
		LOG_ERROR("Could not load shader " << path << "_fs.glsl");
		return -1;
	}

	
	int program = loadShader(vertShaderData.c_str(), fragShaderData.c_str());
	if(program != -1)
	{
		m_shaders.emplace_back(program, path);
	}
	
	return program;
}

int Common::createTexture(Texture* tex)
{
	if(tex->getID() != -1)
		return tex->getID();
	
	auto format = GL_RGBA;
	if(tex->getComponents() == 3)
		format = GL_RGB;

	unsigned int texture = -1;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// TODO Use material settings!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float aniso;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso); 
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, tex->getWidth(), tex->getHeight(), 0, format, GL_UNSIGNED_BYTE, tex->getData());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	m_textures.push_back(texture);
	tex->setID(texture);
	
	return texture;
}

std::string Common::preprocess(const char* path)
{
	char* file = readTextFile(path);
	if(!file)
	{
		LOG_ERROR("Could not open file: " << path);
		throw std::runtime_error("Could not open file: " + std::string(path));
	}

	std::stringstream in(file); // TODO Make readTextFile STL aware!
	delete file;
	
	std::string basePath(path);
	basePath.erase(basePath.find_last_of('/') + 1);
	
	std::stringstream out;
	std::string line;
	
	static const std::regex regex("#include(\\s)+(<|\")(.*)(>|\")");
	
	while(!in.eof())
	{
		std::smatch result;
		std::getline(in, line, '\n');
				
		std::regex_search(line, result, regex);
		
		if(!result.empty())
		{
			std::string file = preprocess((basePath + result[3].str()).c_str());
			if(file.empty())
			{
				LOG_ERROR("Could not load include file: " << basePath << result[3].str());
				throw std::runtime_error("Could not open file: " + basePath + result[3].str());
			}
			out << file;
			continue;
		}
		
		out << line << '\n';
	}
	
	return out.str();
}

int Common::findShader(const char* name)
{
	for(int i = 0; i < m_shaders.size(); i++)
	{
		if(m_shaders[i].name == name)
			return i;
	}
	
	return -1;
}

#define CCPP_IMPL
#include "ccpp.h"

void Common::gatherUniforms(const std::string& code, Shader& shader)
{
	static const std::regex regex("uniform\\s+(\\w+)\\s+([a-zA-Z_]+)(\\s+=\\s+(.*)|);\\s*(//(.*)|)");
	static const std::unordered_map<std::string, PROPERTY_TYPES> typeMap{
		{"int", INTEGER},
		{"float", FLOAT},
		{"vec2", VECTOR2},
		{"vec3", VECTOR3},
		{"vec4", VECTOR4},
		{"mat4", MATRIX4x4},
		{"sampler2D", INTEGER},
		{"sampler3D", INTEGER}
	};
	
	auto uniformIterBegin = std::sregex_iterator(code.begin(), code.end(), regex);
	auto uniformIterEnd = std::sregex_iterator();
	
	// LOG_DEBUG(shader.name.str() << ": Found " << std::distance(uniformIterBegin, uniformIterEnd) << " uniforms.");
	
	for(; uniformIterBegin != uniformIterEnd; uniformIterBegin++)
	{
		auto& match = *uniformIterBegin;
		if(match[6] == " HIDDEN")
			continue;
		
		IProperty* prop;
		auto type = typeMap.at(match[1]);
		auto initializer = match[4].str();
		
		switch(type)
		{
		default: type = INTEGER;
		case INTEGER: prop = new StaticProperty<int>(match[2].str().c_str(), initializer.empty() ? 0 : std::stoi(initializer), type); break;
		case FLOAT: prop = new StaticProperty<float>(match[2].str().c_str(), initializer.empty() ? 0.0f : std::stod(initializer), type); break;
		case VECTOR2: prop = new StaticProperty<Vector2>(match[2].str().c_str(), type); break;
		case VECTOR3: prop = new StaticProperty<Vector3>(match[2].str().c_str(), type); break;
		case VECTOR4: prop = new StaticProperty<Vector4>(match[2].str().c_str(), type); break;
		}
		
		shader.uniforms.push_back({ std::unique_ptr<IProperty>(prop), -1 });
	}
}

void Common::setupMaterial(Material& material, const char* shaderName)
{
	int shaderId = findShader(shaderName);
	if(shaderId != -1)
	{
		// LOG_DEBUG("Found shader in cache for " << shaderName);
		material.setShader(shaderId);
		
		// FIXME Duplicate code!
		// LOG_DEBUG("Cloning " << getShader(shaderId)->uniforms.size() << " properties");
		for(auto& k : getShader(shaderId)->uniforms)
		{
			// LOG_DEBUG("Shader property: " << k.first->getName());
			auto* prop = material.getProperty(k.first->getName().c_str());
			if(!prop)
			{
				prop = k.first->clone();
				material.registerProperty(prop);
			}
			else
			{
				// LOG_DEBUG("Property skipped.");
			}

			// Set ID to zero to indicate it is uninitialized
			prop->setUserId(0);
		}
		return;
	}
	
	// Preprocess sources
	std::string fullPath("assets/materials/");
	fullPath += shaderName;
	fullPath += ".glsl";
	
	std::string vertexShader = preprocess(fullPath.c_str());
	std::string fragmentShader = vertexShader;
	
	ccpp::processor pp;
	pp.set_command_callback([] (const char*, const char*) {
		return true;
	});
	
	pp.add_define("NEO_VERTEX");
	pp.process(&vertexShader[0], vertexShader.size());
	
	pp.remove_define("NEO_VERTEX");
	pp.add_define("NEO_FRAGMENT");
	pp.process(&fragmentShader[0], fragmentShader.size());
	
	vertexShader.erase(vertexShader.find_last_not_of(' ') + 1);
	fragmentShader.erase(fragmentShader.find_last_not_of(' ') + 1);
	
	std::replace(vertexShader.begin(), vertexShader.end(), '$', '#');
	std::replace(fragmentShader.begin(), fragmentShader.end(), '$', '#');
		
	shaderId = m_shaders.size();
	m_shaders.emplace_back();

	Shader& shader = m_shaders.back();
	shader.name = shaderName;
	shader.id = -1;
	
	material.setShader(shaderId);
	
	shader.vertexSource = std::move(vertexShader);
	shader.fragmentSource = std::move(fragmentShader);
		
	// Gather uniforms
	gatherUniforms(shader.vertexSource, shader);
	gatherUniforms(shader.fragmentSource, shader);
	
	// FIXME Duplicate code!
	// LOG_DEBUG("Created a new shader " << shaderName);
	for(auto& k : shader.uniforms)
	{
		// LOG_DEBUG("Shader property: " << k.first->getName());
		auto* prop = material.getProperty(k.first->getName().c_str());
		if(!prop)
		{
			prop = k.first->clone();
			material.registerProperty(prop);
		}
		else
		{
			// LOG_DEBUG("Property skipped.");
		}

		// Set ID to zero to indicate it is uninitialized
		prop->setUserId(0);
	}
}

void Common::compileShaders()
{
	LOG_DEBUG("Compiling shaders");
	for(unsigned int i = 0; i < m_shaders.size(); i++)
	{
		Shader* shader = &m_shaders[i];
		if(shader->id == -1)
		{
			LOG_DEBUG("Building shader " << shader->name.str());
			shader->id = loadShader(shader->vertexSource.c_str(), shader->fragmentSource.c_str());
			
			glUseProgram(shader->id);
			shader->uModelView = glGetUniformLocation(shader->id, "ModelViewMatrix");
			shader->uModelViewProj = glGetUniformLocation(shader->id, "ModelViewProjectionMatrix");
			shader->uNormal = glGetUniformLocation(shader->id, "NormalMatrix");
			shader->uTime = glGetUniformLocation(shader->id, "Time");
			shader->uNumLights = glGetUniformLocation(shader->id, "NumLights");
			
			glUniform1i(glGetUniformLocation(shader->id, "DiffuseTexture"), Material::DIFFUSE);
			glUniform1i(glGetUniformLocation(shader->id, "NormalTexture"), Material::NORMAL);
			glUniform1i(glGetUniformLocation(shader->id, "SpecularTexture"), Material::SPECULAR);
			glUniform1i(glGetUniformLocation(shader->id, "HeightTexture"), Material::HEIGHT);

			shader->uTextureFlags[0] = glGetUniformLocation(shader->id, "HasDiffuse");
			shader->uTextureFlags[1] = glGetUniformLocation(shader->id, "HasNormal");
			shader->uTextureFlags[2] = glGetUniformLocation(shader->id, "HasSpecular");
			shader->uTextureFlags[3] = glGetUniformLocation(shader->id, "HasHeight");
			
			assert(shader->uModelView != -1);
			
			for(auto& prop : shader->uniforms)
			{
				prop.second = glGetUniformLocation(shader->id, prop.first->getName().c_str());
			}
		}
		else
		{
			LOG_DEBUG("Ignore already built shader " << shader->name.str());
		}
	}
	
	glUseProgram(0);
}

void Common::enableMaterialTransform(Neo::Material& material, const Neo::Matrix4x4& transform, const Neo::Matrix4x4& view, const Neo::Matrix4x4& proj)
{
	auto MV = view * transform;
	auto MVP = proj * MV;
	auto N = MV.getInversetranspose();
	
	enableMaterial(material, MV, MVP, N);
}

void Common::enableMaterial(Neo::Material& material, const Neo::Matrix4x4& ModelView, const Neo::Matrix4x4& ModelViewProjection, const Neo::Matrix4x4& Normal)
{
	useShader(material.getShader());
	Shader* shader = getShader(material.getShader());

	switch(material.blendMode)
	{
		default:
		case BLENDING_NONE: 
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);
		break;
		
		case BLENDING_ALPHA:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
			
		case BLENDING_ADD:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
		break;
		
		case BLENDING_SUB:
			glEnable(GL_BLEND);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		break;
		
		case BLENDING_LIGHT:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		break;
		
		case BLENDING_PRODUCT:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		break;
	}
	
	assert(shader->uModelView != -1);
	glUniformMatrix4fv(shader->uModelView, 1, GL_FALSE, ModelView.entries);
	glUniformMatrix4fv(shader->uModelViewProj, 1, GL_FALSE, ModelViewProjection.entries);
	glUniformMatrix4fv(shader->uNormal, 1, GL_FALSE, Normal.entries);
	
#if 0
	for(unsigned short i = 0; i < shader->uniforms.size(); i++)
	{
		int uniform = shader->uniforms[i].second;
		auto& prop = material.getProperties()[i];
		switch(prop->getType())
		{
		default:
		case INTEGER: glUniform1i(uniform, prop->get<int>()); break;
		case FLOAT: glUniform1f(uniform, prop->get<float>()); break;
		case VECTOR2: glUniform2fv(uniform, 1, (const float*) prop->data()); break;
		case VECTOR3: glUniform3fv(uniform, 1, (const float*) prop->data()); break;
		case VECTOR4: glUniform4fv(uniform, 1, (const float*) prop->data()); break;
		}
	}
#endif

	for(auto& prop : material.getProperties())
	{
		int uniform = prop->getUserId();
		if(uniform == -1) // Not found
			continue;
		if(uniform == 0) // Not initialized
		{
			uniform = glGetUniformLocation(shader->id, prop->getName().c_str());
			prop->setUserId(uniform);
			
			if(uniform == -1) // Still not found
				continue;
		}

		switch(prop->getType())
		{
		default:
		case INTEGER: glUniform1i(uniform, prop->get<int>()); break;
		case FLOAT: glUniform1f(uniform, prop->get<float>()); break;
		case VECTOR2: glUniform2fv(uniform, 1, (const float*) prop->data()); break;
		case VECTOR3: glUniform3fv(uniform, 1, (const float*) prop->data()); break;
		case VECTOR4: glUniform4fv(uniform, 1, (const float*) prop->data()); break;
		}
	}
	
	for(unsigned short i = 0; i < Material::TEXTURE_MAX; i++)
	{
		if(material.textures[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, material.textures[i]->getID());
			glUniform1i(shader->uTextureFlags[i], 1);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(shader->uTextureFlags[i], 0);
		}
	}

	glActiveTexture(GL_TEXTURE0);
}
