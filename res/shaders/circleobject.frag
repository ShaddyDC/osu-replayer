precision mediump float;

out vec4 FragColor;

in vec2 localPos;

uniform vec4 circleColor;
uniform uint circleCenter;

const uint circleCenterGradient = 0u;
const uint circleCenterFilled = 1u;
const uint circleCenterHollow = 2u;

const float border_width = 0.07f;
const float smooth_width = 0.05f;

const float gradient_min = 0.3f;
const float gradient_max = 0.6f;
const float gradient_multiplier = (gradient_max - gradient_min);

void main()
{
    float dist = length(localPos);

    if (dist > 1.f) discard;

    vec4 color = circleColor;

    float outer_smooth_alpha = 1.f - smoothstep(1.f - smooth_width, 1.f, dist);
    float inner_smooth_alpha = 1.f;

    if (dist <= 1.f - border_width){
        if (circleCenter == circleCenterGradient){
            float inner_a = gradient_min + (1.f - dist) * gradient_multiplier;
            float outer_a = color.a;
            float interp = smoothstep(1.f - border_width - smooth_width, 1.f - border_width, dist);
            color.a = mix(inner_a, outer_a, interp);
        } else if (circleCenter == circleCenterFilled){
            // leave color at circleColor
        } else if (circleCenter == circleCenterHollow){
            inner_smooth_alpha = smoothstep(1.f - border_width - smooth_width, 1.f - border_width, dist);
        }
    }

    FragColor = vec4(color.xyz, color.a * inner_smooth_alpha * outer_smooth_alpha);
}
