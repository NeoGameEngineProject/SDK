#include <Common.h>
#include <string>
#include <memory>

#include <Texture.h>
#include <FileTools.h>
#include <Log.h>

#include <iostream>
#include <StringTools.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <cassert>

using namespace Neo;

namespace
{
bool checkShaderStatus(GLuint shader)
{
	GLint result = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if(!result)
	{
		LOG_ERROR("Could not compile shader!");
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
	for(auto program : m_shaders)
		glDeleteProgram(program);
}

void Common::useShader(unsigned int id)
{
	assert(id < m_shaders.size());
	glUseProgram(m_shaders[id]);
}

int Common::loadShader(const char* path)
{
	std::string fullpath = path;
	GLuint vertexShader = -1, fragmentShader = -1;

	{
		char* vertShaderData = readTextFile((fullpath + "_vs.glsl").c_str());
		if (!vertShaderData)
		{
			LOG_ERROR("Could not load shader " << path << "_vs.glsl");
			return -1;
		}

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if(!vertexShader)
		{
			LOG_ERROR("Could not create OpenGL vertex shader!");
			delete vertShaderData;
			return -1;
		}

		char* array[] = {vertShaderData};
		glShaderSource(vertexShader, 1, array, nullptr);
		glCompileShader(vertexShader);

		if(!checkShaderStatus(vertexShader))
		{
			delete vertShaderData;
			glDeleteShader(vertexShader);
			return -1;
		}

		delete vertShaderData;
	}

	{
		char* fragShaderData = readTextFile((fullpath + "_fs.glsl").c_str());
		if (!fragShaderData)
		{
			LOG_ERROR("Could not load shader " << path << "_fs.glsl");
			return -1;
		}

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if(!fragmentShader)
		{
			LOG_ERROR("Could not create OpenGL fragment shader!");
			delete fragShaderData;
			return -1;
		}

		char* array[] = {fragShaderData};
		glShaderSource(fragmentShader, 1, array, nullptr);
		glCompileShader(fragmentShader);

		if(!checkShaderStatus(fragmentShader))
		{
			delete fragShaderData;
			glDeleteShader(fragmentShader);
			return -1;
		}
		
		delete fragShaderData;
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
		glDeleteProgram(program);
		program = -1;
		goto done;
	}

done:
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	if(program != -1)
	{
		m_shaders.push_back(program);
	}

	return program;
}

int Common::createTexture(Texture* tex)
{
	auto format = GL_RGBA;
	if(tex->getComponents() == 3)
		format = GL_RGB;

	unsigned int texture = -1;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, tex->getWidth(), tex->getHeight(), 0, format, GL_UNSIGNED_BYTE, tex->getData());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	m_textures.push_back(texture);
	return texture;
}


