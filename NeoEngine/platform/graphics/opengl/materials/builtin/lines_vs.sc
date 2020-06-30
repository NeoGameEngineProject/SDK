$version 400

uniform mat4 ViewProjection;

layout(location = 0) in vec3 Vertex;

void main()
{
	gl_Position = ViewProjection * vec4(Vertex, 1.0);
} 
