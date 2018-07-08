#ifndef NEO_LEVELLOADER_H
#define NEO_LEVELLOADER_H

namespace Neo 
{
class Level;
class LevelLoader
{
public:
	static bool load(Level& level, const char* file, const char* rootNode = nullptr);
};

}

#endif // NEO_LEVELLOADER_H
