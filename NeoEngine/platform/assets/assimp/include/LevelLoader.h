#ifndef NEO_LEVELLOADER_H
#define NEO_LEVELLOADER_H

#include "NeoEngine.h"

namespace Neo 
{
class Renderer;
class Level;
class NEO_ENGINE_EXPORT LevelLoader
{
public:
	static bool load(Level& level, const char* file, const char* rootNode = nullptr);
};

}

#endif // NEO_LEVELLOADER_H
