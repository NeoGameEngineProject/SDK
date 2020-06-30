
vec3 calculateLight(vec3 color, vec3 p, vec3 n, vec3 v, float roughness,
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

	// Gauss constant
	const float c = 1.0;

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
	vec3 retval = max(0.0, nDots) * ((color.rgb * lightColor.rgb) + (lightColor.rgb * Specular.rgb) * rs);

	return attenuation * retval;
}