#ifndef __JSON_SCENE_H__
#define __JSON_SCENE_H__

#include <LevelLoader.h>

namespace Neo
{

class NEO_ENGINE_EXPORT JsonScene : public SceneFile
{
public:
	bool load(Level& level, std::istream& file, const std::string& workingDirectory = "", ObjectHandle root = ObjectHandle()) override;
	bool save(Level& level, std::ostream& file, const std::string& workingDirectory = "", ObjectHandle root = ObjectHandle()) override;

	bool supportsExtension(const std::string& ext) override;
	const char* getName() const override { return "JsonScene"; }
};

NEO_ENGINE_EXPORT std::string encodeData(std::istream& in);
NEO_ENGINE_EXPORT void decodeData(std::stringstream& out, const std::string& str);

}

#endif
