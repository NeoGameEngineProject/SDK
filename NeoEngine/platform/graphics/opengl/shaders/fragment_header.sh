
#include "ShaderLight.h"

// $extension GL_ARB_shading_language_420pack : enable

// ifdef GL_ARB_shading_language_420pack
// layout (std140, binding = 0) uniform Lights
uniform Lights
{
	ShaderLight lights;
};

in vec3 normal;
in vec3 modelNormal;
in vec3 position;
in vec3 modelPosition;
in vec2 texcoord;
in vec3 tangent;
in vec3 bitangent;
in vec3 worldPosition;
in mat3 worldToTangent;

uniform mat4 ModelMatrix; // HIDDEN
uniform vec3 CameraPosition; // HIDDEN

uniform sampler2D DiffuseTexture; // HIDDEN
uniform sampler2D NormalTexture; // HIDDEN
uniform sampler2D SpecularTexture; // HIDDEN
uniform sampler2D HeightTexture; // HIDDEN

uniform int HasDiffuse = 0; // HIDDEN
uniform int HasNormal = 0; // HIDDEN
uniform int HasSpecular = 0; // HIDDEN
uniform int HasHeight = 0; // HIDDEN

uniform int NumTextures; // HIDDEN
uniform int NumLights; // HIDDEN

uniform samplerCube Skybox; // HIDDEN

vec3 SkyboxReflection(float lod)
{
	vec3 R = reflect(modelPosition - CameraPosition, modelNormal); //, 1.0/1.52);
	return textureLod(Skybox, R.xzy, lod).rgb;
}

vec3 SkyboxDiffuse(vec3 normal)
{
	return textureLod(Skybox, normal.xzy, 8).rgb;
}

vec3 SkyboxRefraction(float lod, float amount)
{
	vec3 R = refract(modelPosition - CameraPosition, modelNormal, amount);
	return textureLod(Skybox, R.xzy, lod).rgb;
}
