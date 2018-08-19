#ifndef NEO_LIGHTBEHAVIOR_H
#define NEO_LIGHTBEHAVIOR_H

#include <Behavior.h>
#include <Vector3.h>

namespace Neo
{

class LightBehavior : public Neo::Behavior
{
public:
	const char* getName() const override{ return "Light"; }
	Behavior* getNew() const override { return new LightBehavior; }
	
	float angle = 0.0f;
	float brightness = 1.0f;
	float exponent = 0.0f;
	Vector3 diffuse = Vector3(0.8f, 0.8f, 0.8f);
	Vector3 specular = Vector3(1, 1, 1);
	float attenuation = 0.01;
	
	void serialize(std::ostream& out) override;
	void deserialize(Level&, std::istream& out) override;
};

}

#endif // NEO_LIGHTBEHAVIOR_H

