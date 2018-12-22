$version 400

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;

layout(location = 2) in vec3 Tangent;
layout(location = 3) in vec3 Bitangent;

layout(location = 4) in vec2 TexCoord;

out vec3 normal;
out vec3 position;
out vec2 texcoord;
out vec3 tangent;
out vec3 bitangent;

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 NormalMatrix;

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(Position, 1.0);
	position = (ModelViewMatrix * vec4(Position, 1.0)).xyz;

	normal = mat3(NormalMatrix) * Normal;
	texcoord = TexCoord;
	
	tangent = mat3(NormalMatrix) * Tangent;
	bitangent = mat3(NormalMatrix) * Bitangent;
}
