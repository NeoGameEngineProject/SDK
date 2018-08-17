$version 400

in vec3 normal;
in vec3 position;
in vec2 texcoord;

uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;
uniform sampler2D SpecularTexture;
uniform int NumTextures;

uniform vec3 Diffuse;
uniform vec3 Specular;
uniform vec3 Emit;
uniform float Opacity;
uniform float Shininess;

uniform int NumLights;

#include "ShaderLight.h"
layout (std140) uniform Lights
{
	ShaderLight lights;
};

#include "cook_torrance.sc"

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

void main()
{
	if(NumTextures > 0)
	{
		gl_FragColor = texture2D(DiffuseTexture, texcoord);
	}
	else
	{
		gl_FragColor = vec4(Diffuse, Opacity);
	}

	gl_FragColor.rgb = removeGamma(gl_FragColor.rgb);

	float roughness = 0.4f / Shininess;
	vec3 v = normalize(-position);
	vec3 accumulator = Emit.rgb; // = Ambient + Emissive;

	for(int i = 0; i < NumLights; i++)
	{
		//accumulator += gl_FragColor.rgb * (lights.diffuseBrightness[i].rgb / length(lights.positionExponent[i].xyz - position));

		accumulator += calculateLight(gl_FragColor.rgb, position, normal, v, roughness,
										lights.positionExponent[i],
										lights.diffuseBrightness[i],
										lights.specularAttenuation[i],
										lights.directionAngle[i]);
	}

	gl_FragColor.rgb = applyGamma(accumulator);
}
