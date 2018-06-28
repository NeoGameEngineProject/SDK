$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(albedo, 0);
SAMPLER2D(normal, 1);
SAMPLER2D(position, 2);
SAMPLER2D(material, 3);
SAMPLER2D(depth, 4);

SAMPLER2D(lights, 5);

uniform vec4 deferredConfig;
#define numLights deferredConfig.x

float rand(vec2 seed)
{
	return fract(sin(dot(seed.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 blinn(vec3 _lightDir, vec3 _normal, vec3 _viewDir)
{
	float ndotl = dot(_normal, _lightDir);
	vec3 reflected = _lightDir - 2.0*ndotl*_normal; // reflect(_lightDir, _normal);
	float rdotv = dot(reflected, _viewDir);
	return vec2(ndotl, rdotv);
}

vec3 phong(vec3 color) 
{
	vec4 positionBuffer = texture2D(position, v_texcoord0);
	float shininess = positionBuffer.w;
	
	vec3 p = positionBuffer.xyz;
	vec3 n = texture2D(normal, v_texcoord0).xyz;
	vec3 v = normalize(-p.xyz);
	
	vec3 specular = texture2D(material, v_texcoord0).rgb;
	
	vec3 accumulator = vec3(0, 0, 0); // = Ambient + Emissive;
	for(int i = 0; i < numLights; i++)
	{
		vec4 lightPosition = texelFetch(lights, ivec2(i, 0), 0);
		vec4 lightColor = texelFetch(lights, ivec2(i, 1), 0);
		vec4 lightOption = texelFetch(lights, ivec2(i, 2), 0);
		
		float lightBrightness = lightColor.w;
		float lightAttenuation = lightPosition.w;
		
		vec3 s = normalize(lightPosition.xyz - p.xyz);
		vec3 r = reflect(-s, n.xyz);
		vec3 h = normalize(v + s);
		
		float attenuation = lightBrightness / (1.0 + lightAttenuation * pow(length(lightPosition.xyz - p), 2));
		
		float spotAttenuation = 1.0f;
		if(lightOption.x > 0.0f)
		{
			vec4 lightDirection = texelFetch(lights, ivec2(i, 3), 0);
			float cosU = cos(lightOption.x);
			float cosP = cos(lightOption.y);
			
			float spot = dot(-s, lightDirection.xyz);

			if (spot > cosU)
			{
				spotAttenuation = (spot - cosU) / (cosU - cosP);
			}
			else
				continue;
		}
		else if(lightOption.x == -1.0f)
		{
			s = -texelFetch(lights, ivec2(i, 3), 0).xyz;
			attenuation = lightBrightness;
		}
		
		float sDotN = max(dot(s, n.xyz), 0.0);
		float spec = pow(max(dot(h, n), 0.0), 32 * shininess);
		
		accumulator += spotAttenuation * attenuation * (color.rgb * lightColor.rgb * sDotN + specular * spec * lightColor.rgb);
	}
	
	return accumulator; // + vec3(rand(p.xy)) * 0.02;
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
	/*if(v_texcoord0.x > 0.5f)
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
	}*/
	
	gl_FragColor = texture2D(albedo, v_texcoord0);
	if(gl_FragColor.a == 0.0f)
		return;
	
	vec3 p = texture2D(position, v_texcoord0).xyz;
	vec3 n = normalize(texture2D(normal, v_texcoord0).xyz * 2 - 1);
	vec3 v = normalize(-p.xyz);
	
	vec4 lightPosition = texelFetch(lights, ivec2(0, 0), 0);
	vec3 l = lightPosition.xyz - p;
	vec3 s = normalize(l);
	vec3 h = normalize(v + s);
	
	gl_FragColor *= max(dot(s, n), 0.0);
	return;
#endif

	gl_FragColor = texture2D(albedo, v_texcoord0);
	if(gl_FragColor.a == 0.0f)
		return;
		
	gl_FragColor = vec4(applyGamma(phong(removeGamma(gl_FragColor.rgb))), 1.0f);
}
