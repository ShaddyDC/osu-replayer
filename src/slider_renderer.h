#pragma once
#include "shaders/sliderbody_shader.h"
#include "slider.h"
#include "circleobject_renderer.h"
#include "vertex_generate.h"
#include <Magnum/Shaders/Flat.h>

class Slider_renderer{
public:
    void draw(std::vector<Slider_vert> slider, const float radius);

private:
    Sliderbody_shader shader;
    Circleobject_renderer circle_renderer;
    Magnum::Shaders::Flat2D flat_shader{ Magnum::Shaders::Flat2D::Flag::Textured };
};
