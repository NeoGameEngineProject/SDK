#ifndef COMMON_H
#define COMMON_H

#include <Renderer.h>
#include <vector>

namespace Neo
{
class Texture;
class LightBehavior;
class Level;
}

class Common : public Neo::Renderer
{
public:
	virtual ~Common();
	int loadShader(const char* path);
	int createTexture(Neo::Texture* tex);

	void useShader(unsigned int id);

private:
	std::vector<unsigned int> m_shaders;
	std::vector<unsigned int> m_textures;
};


#endif
