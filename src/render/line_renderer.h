#pragma once

#include "shaders/minimal_shader.h"
#include "vertex_generate.h"

using Line_mesh = Magnum::GL::Mesh;

class Line_renderer {
public:
    static Line_mesh generate_mesh(const std::vector<Magnum::Vector2>& line, float width, const std::vector<Magnum::Color4>& colors);
    void draw(Line_mesh& mesh);

private:
    Minimal_shader shader;
};
