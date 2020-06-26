#pragma once

#include "vertex_generate.h"
#include "shaders/minimal_shader.h"

using Line_mesh = Magnum::GL::Mesh;

class Line_renderer{
public:
    Line_mesh generate_mesh(const Slider_segment& line, const float width, const std::vector<Magnum::Color3>& colors);
    void draw(Line_mesh& mesh);

private:
    Minimal_shader shader;
};
