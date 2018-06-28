$input a_position, a_normal, a_texcoord0
$output v_position, v_normal, v_texcoord0

#include <bgfx_shader.sh>

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
	v_position = mul(u_modelView, vec4(a_position, 1.0)).xyz;
	
	mat4 normalMat = transpose(inverse(u_modelView));
	v_normal = normalize(mul(mat3(normalMat), a_normal).xyz);
	
	v_texcoord0 = a_texcoord0;
}
