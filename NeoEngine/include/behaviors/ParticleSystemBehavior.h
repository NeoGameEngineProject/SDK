#ifndef NEO_PARTICLESYSTEM_H
#define NEO_PARTICLESYSTEM_H

#include <PlatformParticleSystemBehavior.h>

namespace Neo
{

class ParticleSystemBehavior : public Neo::PlatformParticleSystemBehavior
{
public:
	const char* getName() const { return "ParticleSystem"; }
	Neo::Behavior* getNew() const { return new ParticleSystemBehavior; }
	void copyTo(Behavior& destination) const override {}
	
	void serialize(std::ostream&);
	void deserialize(Level&, std::istream&);
};

}

#endif // NEO_PARTICLESYSTEM_H
