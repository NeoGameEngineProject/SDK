#ifdef NEO_VERTEX
#include "vertex_header.glh"

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(Position, 1.0);
	position = (ModelViewMatrix * vec4(Position, 1.0)).xyz;

	normal = mat3(NormalMatrix) * Normal;
	texcoord = TexCoord;
	
	tangent = mat3(NormalMatrix) * Tangent;
	bitangent = mat3(NormalMatrix) * Bitangent;

	modelPosition = (ModelMatrix * vec4(Position, 1.0)).xyz;
	modelNormal = normalize(mat3(ModelMatrix) * Normal);
}

#else

uniform vec3 Diffuse;
uniform vec3 Specular;
uniform vec3 Emit;
uniform float Opacity;
uniform float Roughness;
uniform float Metalness;

#include "fragment_header.glh"
#include "gltf.glh"
#include "gamma.glh"

float rand(vec2 co, float rnd_scale)
{
	vec2 v1 = vec2(92.,80.);
	vec2 v2 = vec2(41.,62.);
	return fract(sin(dot(co.xy ,v1)) + cos(dot(co.xy ,v2)) * rnd_scale);
}

void main()
{
	vec3 Normal = normal;
	vec3 v = normalize(-position);
	
	vec3 RoughnessMetalness = texture(Textures[1], texcoord).rgb;
	vec3 Emissive = Emit.rgb * texture(Textures[4], texcoord).rgb;

	float roughness = RoughnessMetalness.g;
	float metalness = RoughnessMetalness.b;

	gl_FragColor = texture(Textures[0], texcoord);
	gl_FragColor.rgb = removeGamma(gl_FragColor.rgb);
	vec3 accumulator = Emissive + gl_FragColor.rgb * removeGamma(SkyboxReflection(modelNormal, roughness)); // = Ambient + Emissive;

	for(int i = 0; i < NumLights; i++)
	{
		accumulator += calculateLight(gl_FragColor.rgb, position, Normal, v, roughness, metalness,
										lights.positionExponent[i],
										lights.diffuseBrightness[i],
										lights.specularAttenuation[i],
										lights.directionAngle[i]);
	}

	gl_FragColor.rgb = applyGamma(accumulator);
}

#endif
