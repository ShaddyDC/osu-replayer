in vec2 position;
in vec2 vLocalPos;

out vec2 localPos;

void main()
{
    gl_Position = vec4(position, 0, 1); // Note low z to overwrite sliderbodies
	localPos = vLocalPos;
}
