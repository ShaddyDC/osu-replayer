in vec2 position;
in vec4 color;

out vec4 fColor;

layout(std140) uniform projection {
	mat4 projectionMatrix;
};

void main()
{
    vec4 pos = projectionMatrix * vec4(position, 1, 1);
    gl_Position = vec4(pos.xy, 0, 1);
    fColor = color;
}
