
#include "ShaderLight.h"
layout (std140, binding = 0) uniform Lights
{
	ShaderLight lights;
};

in vec3 normal;
in vec3 position;
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
