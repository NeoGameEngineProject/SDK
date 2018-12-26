#ifndef RADIAL_BLUR_H
#define RADIAL_BLUR_H

vec4 radialBlur(vec4 color, vec2 texcoord, sampler2D sampler, float sampleDist,
				float sampleStrength)
{
	float samples[10] = float[](-0.08, -0.05, -0.03, -0.02, -0.01, 0.01, 0.02,
								0.03, 0.05, 0.08);

	vec2 dir = vec2(0.5, 0.5) - texcoord;

	float dist = sqrt(dir.x * dir.x + dir.y * dir.y);

	dir = dir / dist;

	vec4 sum = color;
	for (int i = 0; i < 10; i++)
	{
		sum += texture2D(sampler, texcoord + dir * samples[i] * sampleDist);
	}

	sum *= 1.0 / 11.0;

	float t = dist * sampleStrength;
	t = clamp(t, 0.0, 1.0); // 0 << 1

	return mix(color, sum, t);
}

#endif
