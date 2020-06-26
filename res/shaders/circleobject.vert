in vec2 position;
in vec2 vLocalPos;

out vec2 localPos;

layout(std140) uniform projection {
	mat4 projectionMatrix;
};

void main()
{
    vec4 pos = projectionMatrix * vec4(position, 1, 0);
    gl_Position = vec4(pos.xy, 0, 1);   // Note low z to overwrite sliderbodies
	localPos = vLocalPos;
}
