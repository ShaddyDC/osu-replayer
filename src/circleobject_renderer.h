#pragma once
#include "coordinate_converter.h"
#include "shaders/circleobject_shader.h"

class Circleobject_renderer{
public:
    Circleobject_renderer(const Coordinate_holder& holder);
    void draw(const Magnum::Vector2 position, const float radius);

private:
    Coordinate_converter converter;
    Circleobject_shader shader;
};
