$version 400

uniform sampler2D Texture;

in vec2 texcoord;

void main()
{
	gl_FragColor = texture2D(Texture, texcoord);
}
