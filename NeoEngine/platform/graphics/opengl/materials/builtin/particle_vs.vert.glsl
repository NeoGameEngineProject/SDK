#version 400

uniform mat4 ViewProjection;
uniform mat4 Normal;

layout(location = 0) in vec3 Vertex;
layout(location = 1) in vec3 Color;
layout(location = 2) in vec3 Parameters;

out vec3 parameters;

const float constAtten = 0.9;
const float linearAtten = 0.6;
const float quadAtten = 0.001;

void main(void)
{
	gl_Position = ViewProjection * vec4(Vertex, 1.0);
	parameters = Parameters;
	float dist = length(gl_Position.xyz);
	gl_PointSize = Parameters.x * inversesqrt(constAtten+linearAtten*dist + quadAtten*dist*dist);
} 
