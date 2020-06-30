// Implements a BRDF as described by the gltf spec
// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#appendix-b-brdf-implementation
// which is the Schlick BRDF

$define M_PI 3.1415926535897932384626433832795

vec3 fresnel(vec3 f0, vec3 f90, float VdotH)
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

float V_GGX(float NdotL, float NdotV, float alphaRoughnessSq)
{
    float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

    float GGX = GGXV + GGXL;
    if (GGX > 0.0)
    {
        return 0.5 / GGX;
    }
    return 0.0;
}

float D_GGX(float NdotH, float alphaRoughnessSq)
{
    float f = (NdotH * NdotH) * (alphaRoughnessSq - 1.0) + 1.0;
    return alphaRoughnessSq / (M_PI * f * f);
}

vec3 lambertian(vec3 f0, vec3 f90, vec3 diffuseColor, float VdotH)
{
    return (1.0 - fresnel(f0, f90, VdotH)) * (diffuseColor / M_PI);
}

vec3 metallicBRDF(vec3 f0, vec3 f90, float alphaRoughnessSq, float VdotH, float NdotL, float NdotV, float NdotH)
{
    vec3 F = fresnel(f0, f90, VdotH);
    float Vis = V_GGX(NdotL, NdotV, alphaRoughnessSq);
    float D = D_GGX(NdotH, alphaRoughnessSq);

    return F * Vis * D;
}

vec3 calculateLight(vec3 color, vec3 p, vec3 n, vec3 v, float roughness, float metalness,
					vec4 lightPosition, vec4 lightColor, vec4 lightSpecular, vec4 lightDirection)
{
	float lightExponent = lightPosition.w;
	float lightBrightness = lightColor.w;
	float lightAttenuation = lightSpecular.w;
	float lightAngle = lightDirection.w;

	vec3 l;
	if (lightAngle < 1.0)
		l = lightPosition.xyz - p;
	else
		l = -lightDirection.xyz;

	vec3 s = normalize(l);
	vec3 h = normalize(v + s);

	float attenuation = (lightBrightness / (1.0f + lightAttenuation * pow(length(lightPosition.xyz - p), 2.5)));

	if (lightAngle > 0.0 && lightAngle < 1.0)
	{
		float spot = dot(-s, lightDirection.xyz);

		if (spot > lightAngle)
		{
			spot = clamp(pow(spot - lightAngle, lightExponent), 0.0, 1.0);
			attenuation *= spot;
		}
		else
			return vec3(0.0f, 0.0f, 0.0f);
	}

	float nDoth = dot(n, h);
	float nDotv = dot(n, v);
	float vDoth = dot(v, h);
	float nDots = max(0, dot(n, s));

	const vec3 dielectricSpecular = vec3(0.04, 0.04, 0.04);
	const vec3 black = vec3(0, 0, 0);
	const vec3 f90 = vec3(1, 1, 1);

	float alphaSq = roughness*roughness;
	vec3 f0 = mix(dielectricSpecular, color.rgb, metalness);
	vec3 diffuse = mix(color.rgb * (1.0 - dielectricSpecular.r), black, metalness);

	//vec3 retval = max(0.0, nDots) * ((color.rgb * lightColor.rgb) + (lightColor.rgb * Specular.rgb) * rs);
	// vec3 metallicBRDF(vec3 f0, vec3 f90, float alphaRoughness, float VdotH, float NdotL, float NdotV, float NdotH)
	vec3 retval = lambertian(f0, f90, diffuse, vDoth) + lightSpecular.rgb * metallicBRDF(f0, f90, alphaSq, vDoth, nDots, nDotv, nDoth);

	return attenuation * lightColor.rgb * nDots * retval;
}
