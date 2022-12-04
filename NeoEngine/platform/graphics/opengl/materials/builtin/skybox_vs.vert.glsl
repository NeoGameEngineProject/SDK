#version 400

uniform mat4 ViewProjection;

layout(location = 0) in vec3 Vertex;

out vec3 position;

void main(void)
{
	vec4 pos = ViewProjection * vec4(Vertex, 1.0);
	gl_Position = pos.xyww;
	position = Vertex.xzy;
} 
