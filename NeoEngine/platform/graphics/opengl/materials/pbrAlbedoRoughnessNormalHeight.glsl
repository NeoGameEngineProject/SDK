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

	modelPosition = (ModelMatrix * vec4(Position, 1.0)).xyz;
	modelNormal = mat3(ModelMatrix) * Normal;
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

	vec2 TexCoord = texcoord;
	vec3 vdir = normalize(vec3(dot(v, tangent), dot(v, bitangent), dot(v, normal)));
	float height = texture(HeightTexture, texcoord).x * 0.025 - 0.015;
	vec2 offset = vdir.xy / vdir.z * height;

	offset = vdir.xy * height;
	TexCoord += offset;

	vec3 bump = texture(NormalTexture, TexCoord).xyz * 2.0 - 1.0;
	Normal = vec3(normalize(tangent*bump.x + bitangent*bump.y + normal*bump.z));

	vec3 RoughnessMetalness = texture(SpecularTexture, texcoord).rgb;

	float roughness = RoughnessMetalness.g;
	float metalness = RoughnessMetalness.b;
	
	gl_FragColor = texture(DiffuseTexture, TexCoord);
	gl_FragColor.rgb = removeGamma(gl_FragColor.rgb);
	
	vec3 accumulator = Emit.rgb + gl_FragColor.rgb * removeGamma(SkyboxDiffuse(modelNormal)); // = Ambient + Emissive;
	for(int i = 0; i < NumLights; i++)
	{
		//accumulator += gl_FragColor.rgb * (lights.diffuseBrightness[i].rgb / length(lights.positionExponent[i].xyz - position));

		accumulator += calculateLight(gl_FragColor.rgb, position, Normal, v, roughness, metalness,
										lights.positionExponent[i],
										lights.diffuseBrightness[i],
										lights.specularAttenuation[i],
										lights.directionAngle[i]);
	}

	// Map HDR to LDR
	accumulator = accumulator / (accumulator + vec3(1.0));
	gl_FragColor.rgb = applyGamma(accumulator);
}

#endif
