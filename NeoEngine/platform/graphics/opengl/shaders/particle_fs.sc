$version 400

uniform sampler2D Texture;
in vec3 parameters;

void main()
{
	gl_FragColor = texture2D(Texture, gl_PointCoord);
	gl_FragColor.a = min(gl_FragColor.w, parameters.y);
}
