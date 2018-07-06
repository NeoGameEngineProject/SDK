#ifndef NEO_SOUNDLOADER_H
#define NEO_SOUNDLOADER_H

namespace Neo 
{
class Sound;
class SoundLoader
{
public:
	static bool load(Sound& sound, const char* file);
};

}

#endif
