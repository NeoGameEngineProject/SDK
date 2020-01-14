#ifndef NEO_LEVELLOADER_H
#define NEO_LEVELLOADER_H

namespace Neo 
{
class Renderer;
class Level;
class LevelLoader
{
public:
	static bool load(Level& level, const char* file, Renderer& render, const char* rootNode = nullptr);
};

}

#endif // NEO_LEVELLOADER_H
