#ifndef NEO_SCENELINK_H
#define NEO_SCENELINK_H

#include <Behavior.h>

namespace Neo 
{

class NEO_ENGINE_EXPORT SceneLinkBehavior : public Behavior
{
public:
	SceneLinkBehavior()
	{
		REGISTER_PROPERTY(filename);
	}

	const char* getName() const override { return "SceneLink"; }
	Neo::Behavior* getNew() const override { return new SceneLinkBehavior; }
	void copyTo(Neo::Behavior& destination) const override;
	void serialize(std::ostream&) override;
	void deserialize(Neo::Level&, std::istream&) override;

	std::string getFilename() const { return filename; }
	void setFilename(const std::string& filename) { this->filename = filename; }

private:
	std::string filename;
};

}

#endif // NEO_SKYBOXBEHAVIOR_H
