#ifndef __ASSIMP_SCENE_H__
#define __ASSIMP_SCENE_H__

#include <LevelLoader.h>

namespace Neo
{
class NEO_ENGINE_EXPORT AssimpScene : public SceneFile
{
public:
	bool loadFile(Level& level, const std::string& file, ObjectHandle root = ObjectHandle()) override;
	bool saveFile(Level& level, const std::string& file, ObjectHandle root = ObjectHandle()) override;

	bool load(Level& level, std::istream& file, const std::string& workingDirectory = "", ObjectHandle root = ObjectHandle()) override { return false; }
	bool save(Level& level, std::ostream& file, const std::string& workingDirectory = "", ObjectHandle root = ObjectHandle()) override { return false; }

	bool supportsExtension(const std::string& ext) override;
	const char* getName() const override { return "AssimpScene"; }
};
}

#endif
