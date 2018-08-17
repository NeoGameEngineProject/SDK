$version 400

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;

layout(location = 2) in vec2 TexCoord;

out vec3 normal;
out vec3 position;
out vec2 texcoord;

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 NormalMatrix;

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(Position, 1.0);
	position = (ModelViewMatrix * vec4(Position, 1.0)).xyz;
	normal = mat3(NormalMatrix) * Normal;
	texcoord = TexCoord;
}
