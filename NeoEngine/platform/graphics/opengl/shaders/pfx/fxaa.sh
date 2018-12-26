#ifndef FXAA_SH
#define FXAA_SH

// Based on code found here:
// http://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/
vec3 fxaa(sampler2D textureSampler, vec2 vertTexcoord, vec2 texcoordOffset)
{
	// The parameters are hardcoded for now, but could be
	// made into uniforms to control fromt he program.
	const float FXAA_SPAN_MAX = 3.0;
	const float FXAA_REDUCE_MUL = 1.0 / 8.0;   // 1.0/8.0;
	const float FXAA_REDUCE_MIN = 1.0 / 128.0; //(1.0/128.0);

	vec3 rgbNW = texture(textureSampler,
						 vertTexcoord + (vec2(-1.0, -1.0) * texcoordOffset))
					 .xyz;
	vec3 rgbNE = texture(textureSampler,
						 vertTexcoord + (vec2(+1.0, -1.0) * texcoordOffset))
					 .xyz;
	vec3 rgbSW = texture(textureSampler,
						 vertTexcoord + (vec2(-1.0, +1.0) * texcoordOffset))
					 .xyz;
	vec3 rgbSE = texture(textureSampler,
						 vertTexcoord + (vec2(+1.0, +1.0) * texcoordOffset))
					 .xyz;
	vec3 rgbM = texture(textureSampler, vertTexcoord).xyz;

	const vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(rgbM, luma);

	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float dirReduce =
		max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
			FXAA_REDUCE_MIN);

	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
			  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) *
		  texcoordOffset;

	vec3 rgbA =
		(1.0 / 2.0) *
		(texture(textureSampler, vertTexcoord + dir * (1.0 / 3.0 - 0.5)).xyz +
		 texture(textureSampler, vertTexcoord + dir * (2.0 / 3.0 - 0.5)).xyz);
	vec3 rgbB =
		rgbA * (1.0 / 2.0) +
		(1.0 / 4.0) *
			(texture(textureSampler, vertTexcoord + dir * (0.0 / 3.0 - 0.5))
				 .xyz +
			 texture(textureSampler, vertTexcoord + dir * (3.0 / 3.0 - 0.5))
				 .xyz);
	float lumaB = dot(rgbB, luma);

	if ((lumaB < lumaMin) || (lumaB > lumaMax))
		return rgbA;

	return rgbB;
}

#endif
