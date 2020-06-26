#pragma once

#include <Magnum/Shaders/AbstractVector.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Color.h>

#include "slider.h"

struct Slider_vert{
    Slider_vert(Magnum::Vector3 position, Magnum::Float side): position{position}, side{side}{}
    Magnum::Vector3 position;
    Magnum::Float side;
};

struct Line_vert{
    Line_vert(Magnum::Vector2 position, Magnum::Color3 color): position{position}, color{color}{}
    Magnum::Vector2 position;
    Magnum::Color4 color;
};

std::vector<Slider_vert> vertex_generate(const Slider_segment& slider);

std::vector<Line_vert> line_generate(const std::vector<Magnum::Vector2>& points, const float width, std::vector<Magnum::Color3> colors);
