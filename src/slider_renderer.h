#pragma once
#include "coordinate_converter.h"
#include "shaders/sliderbody_shader.h"
#include "slider.h"
#include "circleobject_renderer.h"
#include "vertex_generate.h"
#include <Magnum/Shaders/Flat.h>

class Slider_renderer{
public:
    Slider_renderer(const Coordinate_holder& holder);
    void draw(std::vector<Slider_vert> slider, const float radius);

private:
    Coordinate_converter converter;
    Sliderbody_shader shader;
    Circleobject_renderer circle_renderer;
    Magnum::Shaders::Flat2D flat_shader;
};
