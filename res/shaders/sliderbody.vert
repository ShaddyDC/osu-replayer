layout (location = 0) in vec3 position;
layout (location = 1) in float side;

layout(std140) uniform projection {
	mat4 projectionMatrix;
};

out float fSide;

void main()
{
    gl_Position = projectionMatrix * vec4(position, 1);
	fSide = side;
};
