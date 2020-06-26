in float side;
in vec3 position;

out float fSide;

layout(std140) uniform projection {
	mat4 projectionMatrix;
};

void main()
{
    vec4 pos = projectionMatrix * vec4(position.xy, 1, 0);
    gl_Position = vec4(pos.xy, position.z, 1);
	fSide = side;
}
