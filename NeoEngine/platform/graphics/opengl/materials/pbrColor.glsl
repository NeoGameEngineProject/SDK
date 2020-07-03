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
uniform float Roughness;
uniform float Metalness;

#include "builtin/fragment_header.glsl"
#include "builtin/gltf.glsl"
#include "builtin/gamma.glsl"

void main()
{
	vec3 Normal = normal;
	vec3 v = normalize(-position);
	
	gl_FragColor = vec4(Diffuse, Opacity);
	gl_FragColor.rgb = removeGamma(gl_FragColor.rgb);

	vec3 accumulator = Emit.rgb + gl_FragColor.rgb * removeGamma(SkyboxDiffuse(modelNormal)); // = Ambient + Emissive;

	for(int i = 0; i < NumLights; i++)
	{
		accumulator += calculateLight(gl_FragColor.rgb, position, Normal, v, Roughness,
										lights.positionExponent[i],
										lights.diffuseBrightness[i],
										lights.specularAttenuation[i],
										lights.directionAngle[i]);
	}

	gl_FragColor.rgb = applyGamma(accumulator);
}

#endif

