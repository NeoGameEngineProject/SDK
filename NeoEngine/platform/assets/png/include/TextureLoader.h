#ifndef NEO_TEXTURELOADER_H
#define NEO_TEXTURELOADER_H

namespace Neo 
{
class Texture;
class TextureLoader
{
public:
	static bool load(Texture& level, const char* file);
};

}

#endif // NEO_TEXTURELOADER_H
