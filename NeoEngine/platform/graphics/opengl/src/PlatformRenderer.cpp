#include <GL/glew.h>

#include "PlatformRenderer.h"

#include <Level.h>
#include <Vector4.h>
#include <behaviors/CameraBehavior.h>
#include <behaviors/LightBehavior.h>
#include <behaviors/MeshBehavior.h>

#include <Log.h>

#define vec4 Neo::Vector4
#include "../shaders/ShaderLight.h"
#undef vec4

// Provide NanoVG
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>

using namespace Neo;

void PlatformRenderer::beginFrame(Neo::CameraBehavior& camera)
{
	camera.enable(m_width, m_height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void PlatformRenderer::beginFrame(Level& level, CameraBehavior& cam)
{
	m_currentCamera = &cam;
	beginFrame(cam);
	level.updateVisibility(cam, m_visibleLights);
}

void PlatformRenderer::setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	m_width = w;
	m_height = h;
	
	glViewport(x, y, w, h);
}

void PlatformRenderer::updateLights(MeshBehavior* mesh)
{
	unsigned short lightCount = 0;
	unsigned short indices[MAX_LIGHTS_PER_OBJECT]; // Here we will gather all light indices into the m_visibleLights field

	gatherLights(m_visibleLights, mesh, indices, MAX_LIGHTS_PER_OBJECT, lightCount);

	// Not required because gatherLights should ensure this, just to be sure we do it anyways
	assert(lightCount <= MAX_LIGHTS_PER_OBJECT);

	glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
	auto buffer = reinterpret_cast<ShaderLight*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));

	size_t i = 0;
	for(i = 0; i < lightCount; i++)
	{
		auto light = m_visibleLights[indices[i]];
		if(light == nullptr)
			break;

		Object* parent = light->getParent();

		buffer->positionExponent[i] = m_currentCamera->getViewMatrix() * parent->getPosition();
		buffer->positionExponent[i].w = light->exponent;

		buffer->diffuseBrightness[i] = light->diffuse;
		buffer->diffuseBrightness[i].w = light->brightness;

		buffer->directionAngle[i] = (m_currentCamera->getViewMatrix() * parent->getTransform() * Vector4(0, 0, -1, 0));
		buffer->directionAngle[i].w = light->angle;

		buffer->specularAttenuation[i] = light->specular;
		buffer->specularAttenuation[i].w = light->attenuation;
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glUniform1i(m_uNumLights, lightCount);
}

void PlatformRenderer::clear(float r, float g, float b, bool depth)
{
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

void PlatformRenderer::endFrame()
{
	m_currentCamera = nullptr;
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void PlatformRenderer::initialize(unsigned int w, unsigned int h, void* ndt, void* nwh, void* ctx)
{
	m_width = w;
	m_height = h;

	glewExperimental = true;
	auto err = glewInit();
	if(err == GLEW_ERROR_NO_GLX_DISPLAY)
	{
		LOG_INFO("No GLX display found: Using Wayland backend.");
	}
	else if(err != GLEW_OK)
	{
		LOG_ERROR("Could not initialize GLEW: " << glewGetErrorString(err));
		return;
	}

	auto version = glGetString(GL_VERSION);
	auto vendor = glGetString(GL_VENDOR);
	auto renderer = glGetString(GL_RENDERER);

	LOG_INFO("Renderer:\t" << renderer);
	LOG_INFO("Version:\t" << version);
	LOG_INFO("Vendor:\t" << vendor);

	auto shader = loadShader("assets/glsl/phong");
	assert(shader != -1 && "Could not load shader!");
	
	glViewport(0, 0, w, h);

	glUseProgram(shader);
	m_uModelView = glGetUniformLocation(shader, "ModelViewMatrix");
	m_uModelViewProj = glGetUniformLocation(shader, "ModelViewProjectionMatrix");
	m_uNormal = glGetUniformLocation(shader, "NormalMatrix");
	
	m_uMaterialDiffuse = glGetUniformLocation(shader, "Diffuse");
	m_uMaterialSpecular = glGetUniformLocation(shader, "Specular");
	m_uMaterialShininess = glGetUniformLocation(shader, "Shininess");
	m_uMaterialOpacity = glGetUniformLocation(shader, "Opacity");
	m_uMaterialEmit = glGetUniformLocation(shader, "Emit");

	m_uDiffuseTexture = glGetUniformLocation(shader, "DiffuseTexture");
	m_uNumTextures = glGetUniformLocation(shader, "NumTextures");

	glUniform1i(m_uDiffuseTexture, 0);
	
	glUniform1i(glGetUniformLocation(shader, "DiffuseTexture"), Material::DIFFUSE);
	glUniform1i(glGetUniformLocation(shader, "NormalTexture"), Material::NORMAL);
	glUniform1i(glGetUniformLocation(shader, "SpecularTexture"), Material::SPECULAR);
	glUniform1i(glGetUniformLocation(shader, "HeightTexture"), Material::HEIGHT);

	m_uTextureFlags[0] = glGetUniformLocation(shader, "HasDiffuse");
	m_uTextureFlags[1] = glGetUniformLocation(shader, "HasNormal");
	m_uTextureFlags[2] = glGetUniformLocation(shader, "HasSpecular");
	m_uTextureFlags[3] = glGetUniformLocation(shader, "HasHeight");
	
	m_uNumLights = glGetUniformLocation(shader, "NumLights");
	glGenBuffers(1, &m_uboLights);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderLight), nullptr, GL_DYNAMIC_DRAW);

	auto uboBind = glGetUniformBlockIndex(shader, "Lights");
	const auto bindingPoint = 2;
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_uboLights);
	glUniformBlockBinding(shader, uboBind, bindingPoint);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_visibleLights.alloc(m_maxVisibleLights);
}

void PlatformRenderer::setTransform(const Matrix4x4& transform)
{
	assert(m_currentCamera);
	auto MV = m_currentCamera->getViewMatrix() * transform;
	auto MVP = m_currentCamera->getProjectionMatrix() * MV;
	auto N = MV.getInversetranspose();

	glUniformMatrix4fv(m_uModelView, 1, GL_FALSE, MV.entries);
	glUniformMatrix4fv(m_uModelViewProj, 1, GL_FALSE, MVP.entries);
	glUniformMatrix4fv(m_uNormal, 1, GL_FALSE, N.entries);
}

void PlatformRenderer::setMaterial(MeshHandle mesh)
{
	auto& material = mesh->getMaterial();
	
	glUniform3fv(m_uMaterialDiffuse, 1, reinterpret_cast<const float*>(&material.diffuseColor));
	glUniform3fv(m_uMaterialSpecular, 1, reinterpret_cast<const float*>(&material.specularColor));
	glUniform3fv(m_uMaterialEmit, 1, reinterpret_cast<const float*>(&material.emitColor));

	glUniform1f(m_uMaterialOpacity, material.opacity);
	glUniform1f(m_uMaterialShininess, material.shininess);

	auto& channels = mesh->getTextureChannels();
	auto numTextures = channels.size();

	glUniform1i(m_uNumTextures, numTextures);

	for(unsigned short i = 0; i < Material::TEXTURE_MAX; i++)
	{
		if(material.textures[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, material.textures[i]->getID());
			glUniform1i(m_uTextureFlags[i], 1);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(m_uTextureFlags[i], 0);
		}
	}

	glActiveTexture(GL_TEXTURE0);
}

void PlatformRenderer::swapBuffers()
{

}

void PlatformRenderer::gatherLights(Array<LightBehavior*>& lights, MeshBehavior* mesh, unsigned short* buffer, unsigned short max, unsigned short& count)
{
	for(size_t i = 0; i < lights.count && lights[i] != nullptr && count < max; i++)
	{
		auto parent = lights[i]->getParent();
		float distance = (mesh->getParent()->getPosition() - parent->getPosition()).getLength() - mesh->getBoundingBox().diameter / 2.0f;
		float radius = sqrt(1.0f / (lights[i]->attenuation * 0.15f)); // 0.15 = Minimum brightness
		
		if(distance <= radius)
		{
			buffer[count] = i;
			count++;
		}
	}
}
