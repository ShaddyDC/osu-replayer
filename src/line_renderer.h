#pragma once

#include "coordinate_converter.h"
#include <Magnum/Shaders/VertexColor.h>
#include "vertex_generate.h"

class Line_renderer{
public:
    Line_renderer(const Coordinate_holder& holder);
    void draw(std::vector<Line_vert> line, const float width);

private:
    Coordinate_converter converter;
    Magnum::Shaders::VertexColor2D shader;
};
