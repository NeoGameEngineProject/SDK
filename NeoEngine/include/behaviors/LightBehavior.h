#ifndef NEO_LIGHTBEHAVIOR_H
#define NEO_LIGHTBEHAVIOR_H

#include <Behavior.h>
#include <Vector3.h>

namespace Neo
{

class LightBehavior : public Neo::Behavior
{
public:
	void copyTo(Behavior& b) const override {}
	const char* getName() const override{ return "Light"; }
	Behavior* getNew() const override { return new LightBehavior; }
	
	LightBehavior()
	{
		REGISTER_PROPERTY(angle);
		REGISTER_PROPERTY(brightness);
		REGISTER_PROPERTY(exponent);
		REGISTER_PROPERTY_TYPE(diffuse, COLOR);
		REGISTER_PROPERTY_TYPE(specular, COLOR);
		REGISTER_PROPERTY(attenuation);
	}
	
	LightBehavior(const LightBehavior& l):
		LightBehavior()
	{
		*this = l;
	}
	
	LightBehavior& operator=(const LightBehavior& l)
	{
		angle = l.angle;
		brightness = l.brightness;
		exponent = l.exponent;
		diffuse = l.diffuse;
		specular = l.specular;
		attenuation = l.attenuation;
		
		return *this;
	}
	
	float angle = 0.0f;
	float brightness = 1.0f;
	float exponent = 0.0f;
	Vector4 diffuse = Vector4(0.8f, 0.8f, 0.8f, 1.0f);
	Vector4 specular = Vector4(1, 1, 1, 1);
	float attenuation = 0.01;
	
	void serialize(std::ostream& out) override;
	void deserialize(Level&, std::istream& out) override;
};

}

#endif // NEO_LIGHTBEHAVIOR_H

