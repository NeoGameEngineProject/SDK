$input a_position, a_normal, a_texcoord0
$output v_position, v_normal, v_texcoord0


#include <bgfx_shader.sh>

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
	v_position = gl_Position.xyz;
	v_normal = mul(u_modelView, vec4(a_normal, 0.0)).xyz;
	v_texcoord0 = a_texcoord0;
}
