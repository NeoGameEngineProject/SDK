$version 400

in vec2 texcoord;

uniform sampler2D Color;
uniform sampler2D Depth;
uniform vec2 FrameSize; // vec2(1/w, 1/h)
uniform vec2 Frustum; // vec2(near, far)
uniform int Time = 0;

#include "pfx/blur.sh"
#include "pfx/fxaa.sh"
#include "pfx/ssao.sh"
#include "pfx/bloom.sh"
#include "gamma.sh"

float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand2(vec2 co, float rnd_scale)
{
	vec2 v1 = vec2(92.,80.);
	vec2 v2 = vec2(41.,62.);
	return fract(sin(dot(co.xy ,v1)) + cos(dot(co.xy ,v2)) * rnd_scale);
}

vec4 toneMap(vec4 hdrColor, float exposure)
{
	vec4 ldrColor = 1.0 - exp2(-hdrColor * exposure);
	ldrColor.a = 1.0f;

	return ldrColor;
}

float calculateLuminosity(vec3 color)
{
	return abs(dot(vec3(0.2126f, 0.7152f, 0.0722f), color));
}

void main()
{
	float depth = texture(Depth, texcoord).x;
	float linearDepth = linearizeDepth(depth, Frustum.x, Frustum.y);
	float invDepth = 1.0f - linearDepth;
	
	gl_FragColor.rgb = fxaa(Color, texcoord, 2*FrameSize);
	// gl_FragColor.rgb = removeGamma(fxaa(Color, texcoord, FrameSize));
	
	if(depth < 1.0f)
	{
		gl_FragColor.rgb *= ssao(gl_FragColor.rgb, linearDepth, int(32*invDepth), 5.0f); //, texcoord, FrameSize * 13.0f));
	}
	
	//float hdrTone = clamp(0.6f/calculateLuminosity(texture(Color, texcoord, 12).rgb), 0.0f, 3.0f);
	// gl_FragColor = toneMap(gl_FragColor, 2.5f);
	// gl_FragColor.rgb = applyGamma(gl_FragColor.rgb);
	gl_FragColor.rgb = bloom(gl_FragColor.rgb, 0.0095, 5.0f, 3);
}
