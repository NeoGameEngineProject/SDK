$version 400

#if NEO_VERTEX
#include "builtin/vertex_header.glsl"

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(Position, 1.0);
	position = (ModelViewMatrix * vec4(Position, 1.0)).xyz;
	normal = mat3(NormalMatrix) * Normal;

	modelPosition = (ModelMatrix * vec4(Position, 1.0)).xyz;
	modelNormal = mat3(ModelMatrix) * Normal;
	texcoord = vec2(0.0);
}

#else

uniform vec3 Diffuse;
uniform vec3 Specular;
uniform vec3 Emit;
uniform float Opacity;
uniform float Shininess;

#include "builtin/fragment_header.glsl"
#include "builtin/cook_torrance.glsl"
#include "builtin/gamma.glsl"

void main()
{
	gl_FragColor = vec4(0.5*normal + vec3(0.5), 1);
}

#endif
