#ifndef NEO_TEXTURELOADER_H
#define NEO_TEXTURELOADER_H

#include "NeoEngine.h"

namespace Neo 
{
class Texture;
class NEO_ENGINE_EXPORT TextureLoader
{
public:
	static bool load(Texture& level, const char* file);
};

}

#endif // NEO_TEXTURELOADER_H
