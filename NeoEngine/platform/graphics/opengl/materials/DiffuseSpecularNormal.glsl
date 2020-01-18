$version 400

#if NEO_VERTEX

#include "builtin/vertex_header.glsl"

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(Position, 1.0);
	position = (ModelViewMatrix * vec4(Position, 1.0)).xyz;

	normal = mat3(NormalMatrix) * Normal;
	texcoord = TexCoord;
	
	tangent = mat3(NormalMatrix) * Tangent;
	bitangent = mat3(NormalMatrix) * Bitangent;
}

#else

$extension GL_ARB_shading_language_420pack : require

uniform vec3 Diffuse;
uniform vec3 Specular;
uniform vec3 Emit;
uniform float Opacity;
uniform float Shininess;

#include "builtin/fragment_header.glsl"
#include "builtin/cook_torrance.glsl"
#include "builtin/gamma.glsl"

float rand(vec2 co, float rnd_scale)
{
	vec2 v1 = vec2(92.0, 80.0);
	vec2 v2 = vec2(41.0, 62.0);
	return fract(sin(dot(co.xy ,v1)) + cos(dot(co.xy ,v2)) * rnd_scale);
}

void main()
{
	vec3 Normal = normal;
	float Roughness = 0.4f / Shininess;
	vec3 v = normalize(-position);
	
	vec3 bump = texture(NormalTexture, texcoord).xyz * 2.0 - 1.0;
	Normal = vec3(normalize(tangent*bump.x + bitangent*bump.y + normal*bump.z));
	gl_FragColor = texture(DiffuseTexture, texcoord);
	
	gl_FragColor.rgb = removeGamma(gl_FragColor.rgb);
	vec3 accumulator = Emit.rgb; // = Ambient + Emissive;

	for(int i = 0; i < NumLights; i++)
	{
		accumulator += calculateLight(gl_FragColor.rgb, position, Normal, v, Roughness,
										lights.positionExponent[i],
										lights.diffuseBrightness[i],
										lights.specularAttenuation[i],
										lights.directionAngle[i]);
	}

	// Add random part to prevent color banding
	gl_FragColor.rgb = applyGamma(accumulator) + 0.025f*rand(position.xz*1000.0f, 1000.0f);
}

#endif
