$version 400

uniform mat4 ViewProjection;

layout(location = 0) in vec3 Vertex;
layout(location = 1) in vec2 TexCoord;

out vec2 texcoord;

void main(void)
{
	gl_Position = ViewProjection * vec4(Vertex, 1.0);
	texcoord = TexCoord;
} 
