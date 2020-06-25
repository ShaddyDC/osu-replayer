in float fSide;

out vec4 FragColor;

uniform float inner;

void main()
{
	vec4 color = vec4(1, 1, 1, 1);

	float diff = 2 * abs(fSide - 0.5f);

	if(diff < 0.95f) {
		if(inner == 0.f) discard;
		color = vec4((1 - diff) * 1 / 3);
		color.a = 0.9;
	} else{
		if(inner == 1.f) discard;
	}

    FragColor = color;
} 
