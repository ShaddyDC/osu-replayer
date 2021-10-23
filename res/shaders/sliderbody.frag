precision mediump float;

in float fSide;

out vec4 FragColor;

const float border_width = 0.07f;
const float smooth_width = 0.05f;

const float gradient_min = 0.3f;
const float gradient_max = 0.6f;
const float gradient_multiplier = (gradient_max - gradient_min);

void main()
{
	vec4 color = vec4(1, 1, 1, 1);

	float diff = 2.f * abs(fSide - 0.5f);

	float outer_smooth_alpha =  1.f - smoothstep(1.f - smooth_width, 1.f, diff);
	float inner_smooth_alpha = 1.f;

	if (diff < 1.f - border_width) {
		float inner_a = gradient_min + (1.f - diff) * gradient_multiplier;
		float outer_a = color.a;
		float interp = smoothstep(1.f - border_width - smooth_width, 1.f - border_width, diff);
		color.a = mix(inner_a, outer_a, interp);
	} else {
		diff = 0.95;
	}

	FragColor = vec4(color.xyz, color.a * inner_smooth_alpha * outer_smooth_alpha);
} 
