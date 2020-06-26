in float side;
in vec3 position;

out float fSide;

void main()
{
    gl_Position = vec4(position, 1);
	fSide = side;
}
