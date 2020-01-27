#ifndef NEO_SKYBOXBEHAVIOR_H
#define NEO_SKYBOXBEHAVIOR_H

#include <PlatformSkyboxBehavior.h>

namespace Neo 
{

class NEO_ENGINE_EXPORT SkyboxBehavior : public PlatformSkyboxBehavior
{
public:
	const char* getName() const override { return "Skybox"; }
	Neo::Behavior* getNew() const override { return new SkyboxBehavior; }
	void copyTo(Neo::Behavior& destination) const override;
	void serialize(std::ostream&) override;
	void deserialize(Neo::Level&, std::istream&) override;

	using PlatformSkyboxBehavior::drawSky;
};

}

#endif // NEO_SKYBOXBEHAVIOR_H
