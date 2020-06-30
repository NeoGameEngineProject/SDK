$version 400

uniform samplerCube Skybox;

in vec3 position;

void main()
{
	gl_FragColor = texture(Skybox, position);
}
