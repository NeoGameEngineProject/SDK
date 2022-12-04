#ifdef NEO_VERTEX
#include "vertex_header.glh"

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

#include "fragment_header.glh"
#include "phong.glh"
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
	// float Roughness = 0.4f / Shininess;
	vec3 v = normalize(-position);
	
	gl_FragColor = texture(DiffuseTexture, texcoord);
	
	gl_FragColor.rgb = removeGamma(gl_FragColor.rgb);
	vec3 accumulator = Emit.rgb + gl_FragColor.rgb * removeGamma(SkyboxDiffuse(modelNormal)); // = Ambient + Emissive;

	for(int i = 0; i < NumLights; i++)
	{
		//accumulator += gl_FragColor.rgb * (lights.diffuseBrightness[i].rgb / length(lights.positionExponent[i].xyz - position));

		accumulator += calculateLight(gl_FragColor.rgb, position, Normal, v, Shininess,
										lights.positionExponent[i],
										lights.diffuseBrightness[i],
										lights.specularAttenuation[i],
										lights.directionAngle[i]);
	}


	gl_FragColor.rgb = applyGamma(accumulator);
}

#endif
