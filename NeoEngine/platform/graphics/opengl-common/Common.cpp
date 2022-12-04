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

#include "Preprocessor.h"

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

	// LOG_DEBUG("Loading shader:\n" << vert << "\n\n" << frag);

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
	Preprocessor pp;

	auto vertShaderData = pp.processFile(fullpath + "_vs.vert.glsl");
	if (vertShaderData.empty())
	{
		LOG_ERROR("Could not load shader " << path << "_vs.vert.glsl");
		return -1;
	}
		
	auto fragShaderData = pp.processFile(fullpath + "_fs.frag.glsl");
	if(fragShaderData.empty())
	{
		LOG_ERROR("Could not load shader " << path << "_fs.frag.glsl");
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
	
	int format = 0;
	switch(tex->getComponents())
	{
		case 1: format = GL_RED; break;
		case 2: format = GL_RG; break;
		case 3: format = GL_RGB; break;
		default:
		case 4: format = GL_RGBA; break;
	}

	int componentType = 0;
	switch(tex->getComponentSize())
	{
		default: LOG_WARNING("Unknown component size: " << tex->getComponentSize());
		case 1: componentType = GL_UNSIGNED_BYTE; break;
		case 2: componentType = GL_UNSIGNED_SHORT; break;
		case 4: componentType = GL_UNSIGNED_INT; break;
	}

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
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, tex->getWidth(), tex->getHeight(), 0, format, componentType, tex->getData());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	m_textures.push_back(texture);
	tex->setID(texture);
	
	return texture;
}

std::string Common::preprocess(const char* path)
{
	Preprocessor proc;
	return proc.processFile(path);
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
	
	std::string fragmentShader = preprocess(fullPath.c_str());
	std::string vertexShader = "#version 400\n#define NEO_VERTEX\n" + fragmentShader;
	fragmentShader = "#version 400\n" + fragmentShader; // TODO Configure that!
	
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
			
			if(shader->id == -1)
			{
				LOG_WARNING("Could not load shader " << shader->name);
				continue;
			}

			glUseProgram(shader->id);
			shader->uModel = glGetUniformLocation(shader->id, "ModelMatrix");
			shader->uModelView = glGetUniformLocation(shader->id, "ModelViewMatrix");
			shader->uModelViewProj = glGetUniformLocation(shader->id, "ModelViewProjectionMatrix");
			shader->uNormal = glGetUniformLocation(shader->id, "NormalMatrix");
			shader->uTime = glGetUniformLocation(shader->id, "Time");
			shader->uNumLights = glGetUniformLocation(shader->id, "NumLights");
			shader->uCameraPosition = glGetUniformLocation(shader->id, "CameraPosition");
			
			// FIXME Communicate number of textures!
			for(int i = 0; i < 6; i++)
			{
				const std::string name = "Textures[" + std::to_string(i) + "]";
				glUniform1i(glGetUniformLocation(shader->id, name.c_str()), i);
			}

			shader->uTextureFlags[0] = glGetUniformLocation(shader->id, "HasDiffuse");
			shader->uTextureFlags[1] = glGetUniformLocation(shader->id, "HasNormal");
			shader->uTextureFlags[2] = glGetUniformLocation(shader->id, "HasSpecular");
			shader->uTextureFlags[3] = glGetUniformLocation(shader->id, "HasHeight");

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
	
	enableMaterial(material, -view.getTranslationPart(), transform, MV, MVP, N);
}

void Common::enableMaterial(Neo::Material& material, const Vector3& cameraPosition, const Neo::Matrix4x4& Model, const Neo::Matrix4x4& ModelView, const Neo::Matrix4x4& ModelViewProjection, const Neo::Matrix4x4& Normal)
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
	
	if(shader->uModel != -1) glUniformMatrix4fv(shader->uModel, 1, GL_FALSE, Model.entries);
	if(shader->uModelView != -1) glUniformMatrix4fv(shader->uModelView, 1, GL_FALSE, ModelView.entries);
	if(shader->uModelViewProj != -1) glUniformMatrix4fv(shader->uModelViewProj, 1, GL_FALSE, ModelViewProjection.entries);
	if(shader->uNormal != -1) glUniformMatrix4fv(shader->uNormal, 1, GL_FALSE, Normal.entries);
	if(shader->uCameraPosition != -1) glUniform3fv(shader->uCameraPosition, 1, cameraPosition);
	
#if 0
	for(unsigned short i = 0; i < shader->uniforms.size(); i++)
	{
		int uniform = shader->uniforms[i].second;
		auto& prop = material.getProperties()[i];
		switch(prop->getType())
		{
		default:
		case INTEGER: glUniform1i(uniform, prop->get<int>()); break;
		case FLOAT: glUniform1f(uniform, prop->get<floa#version 400
t>()); break;
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
		else if(uniform == 0) // Not initialized
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
			//glUniform1i(shader->uTextureFlags[i], 1);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			//glUniform1i(shader->uTextureFlags[i], 0);
		}
	}

	glActiveTexture(GL_TEXTURE0);
}
