#ifndef BLUR_SH
#define BLUR_SH

vec4 blur(vec4 diffuse, vec2 texcoord, sampler2D sampler, float amount, int mipLevel)
{
	if (amount == 0.0)
		return diffuse;

	diffuse += texture2D(sampler, texcoord + vec2(0.01, 0.0) * amount, mipLevel);
	diffuse += texture2D(sampler, texcoord + vec2(-0.01, 0.0) * amount, mipLevel);
	diffuse += texture2D(sampler, texcoord + vec2(0.0, 0.01) * amount, mipLevel);
	diffuse += texture2D(sampler, texcoord + vec2(0.0, -0.01) * amount, mipLevel);
	diffuse += texture2D(sampler, texcoord + vec2(0.007, 0.007) * amount, mipLevel);
	diffuse += texture2D(sampler, texcoord + vec2(-0.007, -0.007) * amount, mipLevel);
	diffuse += texture2D(sampler, texcoord + vec2(0.007, -0.007) * amount, mipLevel);
	diffuse += texture2D(sampler, texcoord + vec2(-0.007, 0.007) * amount, mipLevel);

	return diffuse / vec4(8.0);
}

#endif
