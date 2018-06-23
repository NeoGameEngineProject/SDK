#ifndef NEO_LEVELLOADER_H
#define NEO_LEVELLOADER_H

namespace Neo 
{
class Texture;
class TextureLoader
{
public:
	static bool load(Texture& level, const char* file);
};

}

#endif // NEO_LEVELLOADER_H
