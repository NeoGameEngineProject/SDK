#ifndef GAMMA_SH
#define GAMMA_SH

vec3 removeGamma(vec3 color)
{
	return vec3(pow(color.x, 2.2f),
			pow(color.y, 2.2f),
			pow(color.z, 2.2f));
}

vec3 applyGamma(vec3 color)
{
	return vec3(pow(color.x, 1.0f/2.2f),
			pow(color.y, 1.0f/2.2f),
			pow(color.z, 1.0f/2.2f));
}

#endif
