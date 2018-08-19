#include "behaviors/LightBehavior.h"

#include <ostream>
#include <istream>

using namespace Neo;

REGISTER_BEHAVIOR(LightBehavior)

void LightBehavior::serialize(std::ostream& out)
{
	out.write((char*) &angle, sizeof(angle));
	out.write((char*) &brightness, sizeof(brightness));
	out.write((char*) &exponent, sizeof(exponent));
	out.write((char*) &diffuse, sizeof(diffuse));
	out.write((char*) &specular, sizeof(specular));
	out.write((char*) &attenuation, sizeof(attenuation));
}

void LightBehavior::deserialize(Level&, std::istream& in)
{
	in.read((char*) &angle, sizeof(angle));
	in.read((char*) &brightness, sizeof(brightness));
	in.read((char*) &exponent, sizeof(exponent));
	in.read((char*) &diffuse, sizeof(diffuse));
	in.read((char*) &specular, sizeof(specular));
	in.read((char*) &attenuation, sizeof(attenuation));
}
