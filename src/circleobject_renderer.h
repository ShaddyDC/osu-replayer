#pragma once
#include "shaders/circleobject_shader.h"
#include <Magnum/Math/Vector2.h>

class Circleobject_renderer{
public:
    void draw(const Magnum::Vector2 position, const float radius);

private:
    Circleobject_shader shader;
};
