#ifndef SSAO_H
#define SSAO_H

#include "poisson.sh"

#define PI 3.14159265

float linearizeDepth(float z, float near, float far)
{
	return (2.0 * near) / (far + near - z * (far - near));
}

float sampleLinearDepth(vec2 coord)
{
	return linearizeDepth(texture2D(Depth, coord).x, Frustum.x, Frustum.y);
}

vec3 getNormal(vec2 texcoords) 
{
	vec2 offset1 = vec2(0.0f, FrameSize.y); // vec2(0.0,0.001);
	vec2 offset2 = vec2(FrameSize.x, 0.0f); // vec2(0.001,0.0);
	
	float depth = sampleLinearDepth(texcoords);
	float depth1 = sampleLinearDepth(texcoords + offset1);
	float depth2 = sampleLinearDepth(texcoords + offset2);
	
	vec3 p1 = vec3(offset1, depth1 - depth);
	vec3 p2 = vec3(offset2, depth2 - depth);
	
	vec3 normal = cross(p1, p2);
	normal.z = -normal.z;
	
	return normalize(normal);
}

vec3 getPosition(vec2 texcoord)
{
	float depth = linearizeDepth(texture2D(Depth, texcoord).x, Frustum.x, Frustum.y);
	vec3 clipPos = vec3(
			2.0 * (texcoord.x - FrameSize.x) / Frustum.x - 1.0,
			2.0 * (texcoord.y - FrameSize.y) / Frustum.y - 1.0,
			2.0 * depth - 1.0
		);
		
	return clipPos;
}

vec2 noise(vec2 coord)
{
	return vec2(
		clamp(fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453), 0.0, 1.0) * 2.0 - 1.0,
		clamp(fract(sin(dot(coord, vec2(12.9898, 78.233) * 2.0)) * 43758.5453), 0.0, 1.0) * 2.0 - 1.0
	);
}

vec2 rand3(vec2 coord)
{
	return vec2(
		((fract(1.0 - coord.s * (1.0f/FrameSize.x/2.0)) * 0.25) + (fract(coord.t * (1.0f/FrameSize.y/2.0)) * 0.75)) * 2.0 - 1.0,
		((fract(1.0 - coord.s * (1.0f/FrameSize.x/2.0)) * 0.75) + (fract(coord.t * (1.0f/FrameSize.y/2.0)) * 0.25)) * 2.0 - 1.0
	);
}

float compareDepth(float depth1, float depth2, out float far)
{   
	const float diffArea = 0.3; // For self shadowing
	const float displace = 0.4;
	float area = 2.0;
	float diff = (depth1 - depth2) * 100.0f;

	// Check if we need to apply the self shadowing fix
	if(diff < displace)
		area = diffArea;
	else
		far = 1;
		
	// Gauss!
	return pow(2.7182, -2.0 * (diff - displace) * (diff - displace) / (area * area));
}   

float calculateAO(float depth, float dw, float dh, float ssaoRadius)
{   
	float dd = (1.0 - depth) * ssaoRadius;
	
	float ao = 0.0;
	float coordw = texcoord.x + dw*dd;
	float coordh = texcoord.y + dh*dd;
	float coordw2 = texcoord.x - dw*dd;
	float coordh2 = texcoord.y - dh*dd;
	
	vec2 coord = vec2(coordw, coordh);
	vec2 coord2 = vec2(coordw2, coordh2);
	
	float far = Frustum.y;
	
	// far might change!
	ao = compareDepth(depth, sampleLinearDepth(coord), far);

	if (far > 0)
	{
		float newAO = compareDepth(sampleLinearDepth(coord2), depth, far);
		ao += (1.0 - ao) * newAO;
	}
	
	return ao;
} 

vec3 ssao(vec3 color, float depth, int ssaoSamples, float ssaoRadius)
{
	const float ssaoLumInfluence = 0.05;
	const float screenClamp = 0.45;
	const float noiseAmount = 0.0001;

	const bool mist = true;
	const float mistStart = 0.0;
	const float mistEnd = 32.0;
	
	vec2 noise = rand3(texcoord) * noiseAmount;
	float w = FrameSize.x / clamp(depth, screenClamp, 1.0) + (noise.x * (1.0 - noise.x));
	float h = FrameSize.y / clamp(depth, screenClamp, 1.0) + (noise.y * (1.0 - noise.y));
	
	float ao = 0.0;
	
	const float dl = PI * (3.0 - sqrt(5.0));
	float dz = 1.0 / float(ssaoSamples);
	float l = 0.0;
	float z = 1.0 - dz/2.0;
	
	for(int i = 0; i <= ssaoSamples; i++)
	{     
		float r = sqrt(1.0 - z);
		float pw = cos(l) * r;
		float ph = sin(l) * r;
		
		ao += calculateAO(depth, pw * w, ph * h, ssaoRadius);        
		
		z = z - dz;
		l = l + dl;
	}
	
	ao /= float(ssaoSamples);
	ao = 1.0 - ao;
	
	if(mist)
		ao = mix(ao, 1.0, clamp((depth - mistStart) / mistEnd, 0.0, 1.0));
	
	float lum = dot(color, vec3(0.299, 0.587, 0.114));
	vec3 luminance = vec3(lum, lum, lum);
	
	return mix(vec3(ao), vec3(1.0f), luminance * ssaoLumInfluence);
}

#endif
