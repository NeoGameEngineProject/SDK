#include <GL/glew.h>

#include "PlatformRenderer.h"

#include <Level.h>
#include <Vector4.h>
#include <behaviors/CameraBehavior.h>
#include <behaviors/LightBehavior.h>
#include <behaviors/MeshBehavior.h>
#include <behaviors/SkyboxBehavior.h>

#include <AABBRenderer.h>

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
	resetStatistics();
	camera.enable(m_width, m_height);

	auto* skybox = camera.getParent()->getBehavior<SkyboxBehavior>();
	if(skybox)
		skybox->drawSky(this);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void PlatformRenderer::beginFrame(Level& level, CameraBehavior& cam)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_pfxFBO);
	
	m_currentCamera = &cam;
	beginFrame(cam);
	level.updateVisibility(cam, m_visibleLights);
}

void PlatformRenderer::setViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	if(m_width != w || m_height != h)
	{
		if(m_pfxFBO != -1)
			glDeleteFramebuffers(1, &m_pfxFBO);
		
		if(m_pfxDepthTexture != -1)
			glDeleteTextures(1, &m_pfxDepthTexture);
		
		if(m_pfxTexture != -1)
			glDeleteTextures(1, &m_pfxTexture);
		
		glGenFramebuffers(1, &m_pfxFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_pfxFBO);

		/*glGenRenderbuffers(1, &m_pfxDepthTexture);
		glBindRenderbuffer(GL_RENDERBUFFER, m_pfxDepthTexture);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_pfxDepthTexture);*/
		
		glGenTextures(1, &m_pfxDepthTexture);
		glBindTexture(GL_TEXTURE_2D, m_pfxDepthTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_pfxDepthTexture, 0);

		glGenTextures(1, &m_pfxTexture);
		glBindTexture(GL_TEXTURE_2D, m_pfxTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
				
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pfxTexture, 0);
		
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			LOG_ERROR("Could not create post effects framebuffer!");
			m_pfxFBO = 0;
			exit(1); // FIXME: Maybe just fall back on non-fx rendering?
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_currentFBO);
		
		glUseProgram(m_pfxShader);
		glUniform2f(glGetUniformLocation(m_pfxShader, "FrameSize"), 1.0f/w, 1.0f/h);
	}
	
	m_width = w;
	m_height = h;
	
	glViewport(x, y, w, h);
}

void PlatformRenderer::updateLights(MeshBehavior* mesh)
{
	m_visibleLightCount = 0;
	unsigned short indices[MAX_LIGHTS_PER_OBJECT]; // Here we will gather all light indices into the m_visibleLights field

	gatherLights(m_visibleLights, mesh, indices, MAX_LIGHTS_PER_OBJECT, m_visibleLightCount);

	// Not required because gatherLights should ensure this, just to be sure we do it anyways
	assert(m_visibleLightCount <= MAX_LIGHTS_PER_OBJECT);

	glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
	auto buffer = reinterpret_cast<ShaderLight*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));

	size_t i = 0;
	for(i = 0; i < m_visibleLightCount; i++)
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
}

void PlatformRenderer::clear(float r, float g, float b, bool depth)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_pfxFBO);

	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | (depth ? GL_DEPTH_BUFFER_BIT : 0));
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned long long PlatformRenderer::getTime()
{
	using namespace std::chrono;
	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
	);
	
	return ms.count();
}

void PlatformRenderer::endFrame()
{
	// Sort lists
	// FIXME Render opaque and transparent objects separately
	// FIXME Sort by shader
	std::sort(m_opaqueObjects.begin(), m_opaqueObjects.end(), [this](Object* o1, Object* o2) {
		const auto camPos = getCurrentCamera()->getParent()->getPosition();
		return (o1->getPosition() - camPos).getLength() >= (o2->getPosition() - camPos).getLength();
	});
	
	// Draw sorted lists
	for(auto* o : m_opaqueObjects)
	{
		o->draw(*this);
	}

#ifdef DRAW_AABB
	for(auto* o : m_opaqueObjects)
	{
		m_aabbRenderer->drawBox(*this, o->getTransformedBoundingBox());
	}
#endif
	
	m_opaqueObjects.clear();
	
	// Finish frame 
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_currentFBO);
	
	useShader(0);
	
	glUniform2f(m_pfxUFrustum, m_currentCamera->getNear(), m_currentCamera->getFar());
	glUniform1i(m_pfxTime, getTime() - m_startTime);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pfxTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pfxDepthTexture);
	
	glBindVertexArray(m_pfxVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	glBindVertexArray(0);
	
	m_currentCamera = nullptr;
}

void PlatformRenderer::initialize(unsigned int w, unsigned int h, void* ndt, void* nwh, void* ctx)
{
	m_startTime = getTime();
	m_opaqueObjects.reserve(128);
	
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

	glViewport(0, 0, w, h);

	glGenBuffers(1, &m_uboLights);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderLight), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_visibleLights.alloc(m_maxVisibleLights);
	
	// Create FBO for post fx
	{
		m_pfxShader = loadShader("assets/materials/builtin/pfx");
		glUseProgram(m_pfxShader);
		glUniform1i(glGetUniformLocation(m_pfxShader, "Color"), 0);
		glUniform1i(glGetUniformLocation(m_pfxShader, "Depth"), 1);
		
		m_pfxUFrustum = glGetUniformLocation(m_pfxShader, "Frustum");
		m_pfxTime = glGetUniformLocation(m_pfxShader, "Time");

		setViewport(0, 0, w, h);
		
		glGenVertexArrays(1, &m_pfxVAO);
		glBindVertexArray(m_pfxVAO);
		
		glGenBuffers(1, &m_pfxVBO);
		
		Vector2 quad[12] = {
			
			// Vertices
			Vector2(-1, -1),
			Vector2(1, -1),
			Vector2(1, 1),

			Vector2(-1, -1),
			Vector2(-1, 1),
			Vector2(1, 1),
			
			// Tecoords
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 0),
			Vector2(0, 1),
			Vector2(1, 1),
		};

		glBindBuffer(GL_ARRAY_BUFFER, m_pfxVBO);
		glBufferData(GL_ARRAY_BUFFER,
					12*sizeof(Vector2),
					quad,
					GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr); // position

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(sizeof(Vector2)*6)); // texcoord

		glBindVertexArray(0);
	}
	
	compileShaders();
	
#ifdef DRAW_AABB
	m_aabbRenderer = new AABBRenderer();
	m_aabbRenderer->begin(*this);
#endif
}

void PlatformRenderer::compileShaders()
{
	Common::compileShaders();
	
	for(auto& shader : getShaders())
	{
		glUseProgram(shader.id);
		auto uboBind = glGetUniformBlockIndex(shader.id, "Lights");
		if(uboBind == -1)
		{
			LOG_DEBUG("No light UBO found for " << shader.name.str());
			continue;
		}
		
		const auto bindingPoint = 2;
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_uboLights);
		glUniformBlockBinding(shader.id, uboBind, bindingPoint);
	}
}

void PlatformRenderer::enableMaterial(Neo::Material& material, const Neo::Matrix4x4& ModelView, const Neo::Matrix4x4& ModelViewProjection, const Neo::Matrix4x4& Normal)
{
	Common::enableMaterial(material, ModelView, ModelViewProjection, Normal);
	
	auto* shader = getShader(material.getShader());
	glUniform1i(shader->uNumLights, m_visibleLightCount);
}

void PlatformRenderer::draw(Object* object)
{
	m_opaqueObjects.push_back(object);
}

void PlatformRenderer::swapBuffers()
{

}

void PlatformRenderer::gatherLights(Array<LightBehavior*>& lights, MeshBehavior* mesh, unsigned short* buffer, unsigned short max, unsigned short& count)
{
	for(size_t i = 0; i < lights.count && lights[i] != nullptr && count < max; i++)
	{
		auto parent = lights[i]->getParent();
		float distance = (mesh->getParent()->getPosition() - parent->getPosition()).getLength() - mesh->getBoundingBox().getDiameter() / 2.0f;
		float radius = sqrt(1.0f / (lights[i]->attenuation * 0.15f)); // 0.15 = Minimum brightness
		
		if(distance <= radius)
		{
			buffer[count] = i;
			count++;
		}
	}
}
