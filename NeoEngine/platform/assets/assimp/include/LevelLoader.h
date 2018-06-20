#ifndef NEO_LEVELLOADER_H
#define NEO_LEVELLOADER_H

namespace Neo 
{
class Level;

class LevelLoader
{
public:
	static bool loadLevel(Level& level, const char* file);
};

}

#endif // NEO_LEVELLOADER_H
