$input v_position, v_normal, v_texcoord0

#include <bgfx_shader.sh>

uniform sampler2D diffuseTexture;

uniform vec4 textureConfig;
uniform vec4 diffuse;
uniform vec4 specular;

#define u_numTextures textureConfig.x
#define u_shininess textureConfig.y

void main()
{

	gl_FragData[1] = vec4(v_normal, 1.0f);
	gl_FragData[2] = vec4(v_position, u_shininess);
	gl_FragData[3] = specular;
	
	if(u_numTextures == 0)
	{
		gl_FragData[0] = vec4(diffuse.xyz, 1.0f);
	}
	else
	{
		gl_FragData[0] = texture2D(diffuseTexture, v_texcoord0);
	}
}
