#ifndef __SHADER_LIGHT_H__
#define __SHADER_LIGHT_H__

#define SHADER_NUM_LIGHTS 64
struct ShaderLight
{
	vec4 positionExponent[SHADER_NUM_LIGHTS];
	vec4 directionAngle[SHADER_NUM_LIGHTS];
	vec4 diffuseBrightness[SHADER_NUM_LIGHTS];
	vec4 specularAttenuation[SHADER_NUM_LIGHTS];
};

#endif
