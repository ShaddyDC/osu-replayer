#pragma once
#include "shaders/circleobject_shader.h"
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector2.h>

using Circleobject_mesh = Magnum::GL::Mesh;

struct Circle_draw_options {
    Magnum::Color4 color = Magnum::Color4{1};
    Circleobject_shader::Circle_center circle_center = Circleobject_shader::gradient;
};

class Circleobject_renderer {
public:
    [[nodiscard]] static Circleobject_mesh generate_mesh(Magnum::Vector2 position, float radius);
    void draw(Circleobject_mesh& mesh, Circle_draw_options options = Circle_draw_options{});

private:
    Circleobject_shader shader;
};
