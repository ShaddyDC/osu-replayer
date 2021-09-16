precision mediump float;//Todo: Investigate if should be lower

out vec4 FragColor;

in vec2 localPos;

uniform vec4 circleColor;

void main()
{
    float dist = length(localPos);
    vec4 color = circleColor;

    if (dist > 1.f) discard;
    if (dist <= 0.95f){
        color = vec4((1.f - dist) * 1.f / 3.f);
        color.a = 0.7f;
    }

    FragColor = color;
}
