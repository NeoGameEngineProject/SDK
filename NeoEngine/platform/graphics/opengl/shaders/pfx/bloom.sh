#ifndef BLOOM_SH
#define BLOOM_SH

#include "poisson.sh"
#include "../gamma.sh"

vec3 bloom(vec3 pxl, float bloomStrength, float bloomRadius, int mipLevel)
{
	float avg = ((pxl.r + pxl.g + pxl.b) / 3.0);
	vec3 outcolor;
	
	vec3 sum = vec3(0.0f);
	for (int i = -5; i <= 5; i++)
	{
		for (int j = -5; j <= 5; j++)
		{
			vec3 color = texture(Color, (texcoord + bloomRadius*vec2(i * FrameSize.x, j * FrameSize.y)), mipLevel).rgb;
			sum += color * bloomStrength; // 0.015;
		}
	}

	if (avg < 0.025)
	{
		outcolor = pxl + sum * 0.335;
	}
	else if (avg < 0.10)
	{
		outcolor = pxl + (sum * sum) * 0.5;
	}
	else if (avg < 0.88)
	{
		outcolor = pxl + ((sum * sum) * 0.333);
	}
	else if (avg >= 0.88)
	{
		outcolor = pxl + sum;
	}
	else
	{
		outcolor = pxl;
	}
	
	return outcolor;
}

#endif
