$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(albedo, 0);
SAMPLER2D(normal, 1);
SAMPLER2D(position, 2);
SAMPLER2D(depth, 3);

void main()
{
#if 0
	if(v_texcoord0.x > 0.5f)
	{
		if(v_texcoord0.y > 0.5f)
		{
			gl_FragColor = texture2D(position, v_texcoord0);
			return;
		}
		else
		{
			gl_FragColor = texture2D(normal, v_texcoord0);
			return;
		}
	}
	
	gl_FragColor = texture2D(albedo, v_texcoord0);
#endif

	gl_FragColor = texture2D(albedo, v_texcoord0);
	if(gl_FragColor.a == 0.0f)
		return;
}
