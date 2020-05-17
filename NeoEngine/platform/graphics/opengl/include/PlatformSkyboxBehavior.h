#ifndef NEO_PLATFORMSKYBOXBEHAVIOR_H
#define NEO_PLATFORMSKYBOXBEHAVIOR_H

#include <Behavior.h>

namespace Neo
{

class Texture;
class NEO_ENGINE_EXPORT PlatformSkyboxBehavior : public Neo::Behavior
{
public:
	void drawSky(PlatformRenderer* prender);
	void begin(Neo::Platform&, Neo::Renderer&, Level&) override;

	int bindSkybox(int sampler = 31);
	
private:
	Texture* m_textures[6] = {nullptr};
	unsigned int m_shader = -1, m_vao = -1, m_vbo = -1;
	unsigned int m_uViewProjection = -1;
	
	unsigned int m_cubeMap = -1;

	FixedString<128> TextureBase = "assets/skybox/";
};

}

#endif // NEO_PLATFORMSKYBOXBEHAVIOR_H
