precision mediump float;	//Todo: Investigate if should be lower

in float fSide;

out vec4 FragColor;

void main()
{
	vec4 color = vec4(1, 1, 1, 1);

	float diff = 2.f * abs(fSide - 0.5f);

	if(diff < 0.95) {
		color = vec4((1.f - diff) * 1.f / 3.f);
		color.a = 0.7;
	} else{
		diff = 0.95;
	}

    FragColor = color;
} 
