$input v_position, v_normal, v_texcoord0

#include <bgfx_shader.sh>

struct Material
{
	// TODO Blend mode
	float opacity;
	float shininess;
	float customValue;
	vec3 diffuse;
	vec3 specular;
	vec3 emit;
	vec3 customColor;
};

uniform vec4 diffuse;
uniform sampler2D diffuseTexture;
uniform vec4 textureConfig;

#define u_numTextures textureConfig.x

void main()
{

	gl_FragData[1] = vec4(v_normal, 1.0);
	gl_FragData[2] = vec4(v_position, 1.0f);

	if(u_numTextures == 0)
	{
		gl_FragData[0] = vec4(diffuse.xyz, 1.0f);
	}
	else
	{
		gl_FragData[0] = texture2D(diffuseTexture, v_texcoord0);
	}
}
