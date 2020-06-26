#pragma once

#include "vertex_generate.h"
#include "shaders/minimal_shader.h"

class Line_renderer{
public:
    void draw(std::vector<Line_vert> line, const float width);

private:
    Minimal_shader shader;
};
