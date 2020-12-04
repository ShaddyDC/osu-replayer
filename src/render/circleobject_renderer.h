#pragma once
#include "shaders/circleobject_shader.h"
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Vector2.h>

using Circleobject_mesh = Magnum::GL::Mesh;

class Circleobject_renderer {
public:
    [[nodiscard]] static Circleobject_mesh generate_mesh(const Magnum::Vector2 position, const float radius) ;
    void draw(Circleobject_mesh& mesh);

private:
    Circleobject_shader shader;
};
