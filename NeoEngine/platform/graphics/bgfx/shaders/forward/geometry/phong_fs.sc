$input v_position, v_normal, v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(albedo, 0);
SAMPLER2D(normal, 1);

uniform sampler2D diffuseTexture;

uniform vec4 u_config;
#define u_numLights int(u_config.x)

uniform vec4 textureConfig;
uniform vec4 u_diffuse;
uniform vec4 u_specular;

#define u_opacity u_diffuse.a;

uniform vec4 u_lightPositions[8];
uniform vec4 u_lightColors[8];
uniform vec4 u_lightDirections[8];
uniform vec4 u_lightOptions[8];

#define u_numTextures textureConfig.x
#define u_shininess textureConfig.y

vec3 cookTorranceSpecular(vec3 color, vec3 p, vec3 n, vec3 v, float roughness,
				vec4 lightPosition, vec4 lightColor, vec4 lightOption, vec4 lightDirection)
{
	float lightBrightness = lightColor.w;
	float lightAttenuation = lightPosition.w;
	
	vec3 l;
	if (lightOption.x < 1.0)
		l = lightPosition.xyz - p;
	else
		l = -lightDirection.xyz;

	// Gauss constant
	const float c = 1.0;

	vec3 s = normalize(l);
	vec3 h = normalize(v + s);

	float attenuation = (lightBrightness / (1.0f + lightAttenuation * pow(length(lightPosition.xyz - p), 2.5)));

	if (lightOption.x > 0.0 && lightOption.x < 1.0)
	{
		float spot = dot(-s, lightDirection.xyz);

		if (spot > lightOption.x)
		{
			spot = clamp(pow(spot - lightOption.x, lightOption.y), 0.0, 1.0);
			attenuation *= spot;
		}
		else
			return vec3(0.0f, 0.0f, 0.0f);
	}

	
	float nDoth = dot(n, h);
	float nDotv = dot(n, v);
	float vDoth = dot(v, h);
	float nDots = dot(n, s);

	float Geometric =
		min(1.0, min((2.0f * nDoth * nDotv) / vDoth, (2.0f * nDoth * nDots) / vDoth));

	float alpha = acos(nDoth);
	float Roughness = c * exp(-(alpha / (roughness * roughness)));

	const float normIncidence = 1.0;
	float F0 = 0.04f;
	float Fresnel = F0 + pow(1.0f - vDoth, 5.0f) * (1.0f - F0);
	Fresnel *= (1.0f - normIncidence);
	Fresnel += normIncidence;

	float numerator = (Fresnel * Geometric * Roughness);
	float denominator = nDotv * nDots;
	float rs = numerator / denominator;
	vec3 retval = max(0.0, nDots) * ((color.rgb * lightColor.rgb) + (lightColor.rgb * u_specular.rgb) * rs);
	
	return attenuation * retval;
}


vec3 removeGamma(vec3 color)
{
	return vec3(pow(color.x, 2.2f), 
			pow(color.y, 2.2f),
			pow(color.z, 2.2f));
}

vec3 applyGamma(vec3 color)
{
	return vec3(pow(color.x, 1.0f/2.2f), 
			pow(color.y, 1.0f/2.2f),
			pow(color.z, 1.0f/2.2f));
}

void main()
{
#if 0
	if(v_texcoord0.x > 0.5f)
	{
		if(v_texcoord0.y > 0.5f)
		{
			gl_FragColor = 0.05*texture2D(position, v_texcoord0);
			return;
		}
		else
		{
			gl_FragColor = texture2D(normal, v_texcoord0);
			return;
		}
	}
	
	gl_FragColor = texture2D(albedo, v_texcoord0);
	return;
#endif

	if(u_numTextures == 0.0f)
	{
		gl_FragColor = vec4(u_diffuse.xyz, 1.0f);
	}
	else
	{
		gl_FragColor = texture2D(diffuseTexture, v_texcoord0);
	}
	
	gl_FragColor.rgb = removeGamma(gl_FragColor.rgb);
	
	float roughness = 0.4f / u_shininess;
	vec3 v = normalize(-v_position);
	vec3 accumulator = vec3(0, 0, 0); // = Ambient + Emissive;
	
	if(u_numLights >= 1)
	{
		accumulator += cookTorranceSpecular(gl_FragColor.rgb, v_position, v_normal, v, roughness,
			u_lightPositions[0],
			u_lightColors[0],
			u_lightOptions[0],
			u_lightDirections[0]
		);

		if(u_numLights >= 2)
		{
			accumulator += cookTorranceSpecular(gl_FragColor.rgb, v_position, v_normal, v, roughness,
				u_lightPositions[1],
				u_lightColors[1],
				u_lightOptions[1],
				u_lightDirections[1]
			);

			if(u_numLights >= 3)
			{
				accumulator += cookTorranceSpecular(gl_FragColor.rgb, v_position, v_normal, v, roughness,
					u_lightPositions[2],
					u_lightColors[2],
					u_lightOptions[2],
					u_lightDirections[2]
				);

				if(u_numLights >= 4)
				{
					accumulator += cookTorranceSpecular(gl_FragColor.rgb, v_position, v_normal, v, roughness,
						u_lightPositions[3],
						u_lightColors[3],
						u_lightOptions[3],
						u_lightDirections[3]
					);
					
					if(u_numLights >= 5)
					{
						accumulator += cookTorranceSpecular(gl_FragColor.rgb, v_position, v_normal, v, roughness,
							u_lightPositions[4],
							u_lightColors[4],
							u_lightOptions[4],
							u_lightDirections[4]
						);
						
						if(u_numLights >= 6)
						{
							accumulator += cookTorranceSpecular(gl_FragColor.rgb, v_position, v_normal, v, roughness,
								u_lightPositions[5],
								u_lightColors[5],
								u_lightOptions[5],
								u_lightDirections[5]
							);
						}
					}
				}

			}
		}
	}
	gl_FragColor.rgb = applyGamma(accumulator.rgb);
	gl_FragColor.a = u_opacity;
}
