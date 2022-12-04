#version 400

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;

out vec2 texcoord;

void main()
{
	gl_Position = vec4(Position, 1.0f, 1.0f);
	texcoord = TexCoord;
}
