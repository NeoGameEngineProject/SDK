#ifndef __ASSIMP_SCENE_H__
#define __ASSIMP_SCENE_H__

#include <LevelLoader.h>

namespace Neo
{
class NEO_ENGINE_EXPORT AssimpScene : public SceneFile
{
public:
	bool load(Level& level, const std::string& file, ObjectHandle root = ObjectHandle()) override;
	bool save(Level& level, const std::string& file, ObjectHandle root = ObjectHandle()) override;

	bool load(Level& level, std::istream& file, ObjectHandle root = ObjectHandle()) override { return false; }
	bool save(Level& level, std::ostream& file, ObjectHandle root = ObjectHandle()) override { return false; }

	bool supportsExtension(const std::string& ext) override;
	const char* getName() const override { return "AssimpScene"; }
};
}

#endif
