precision mediump float;	//Todo: Investigate if should be lower

in vec2 localPos;

out vec4 FragColor;

void main()
{
	float dist = length(localPos);
	vec4 color = vec4(1);

	if(dist > 1.f) discard;
    if(dist <= 0.95f){
        color = vec4((1.f - dist) * 1.f / 3.f);
		color.a = 0.7f;
    }

    FragColor = color;
}
