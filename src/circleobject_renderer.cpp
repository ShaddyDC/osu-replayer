#include "circleobject_renderer.h"
#include <Magnum/GL/Buffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/GL/Mesh.h>

struct Circle_vertex{
    Magnum::Vector2 position;
    Magnum::Vector2 local_position;
};

void Circleobject_renderer::draw(const Magnum::Vector2 position, const float radius)
{
    Circle_vertex data[] = {
        { position + Magnum::Vector2{ -radius, -radius }, { -1, -1 }},
        { position + Magnum::Vector2{ -radius, radius }, { -1, 1 }},
        { position + Magnum::Vector2{ radius, -radius }, { 1, -1 }},
        { position + Magnum::Vector2{ radius, radius }, { 1, 1 }},
    };

    Magnum::GL::Buffer buffer {};
    buffer.setData(data);
    Magnum::GL::Mesh mesh;
    mesh.setCount(4)
        .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
        .addVertexBuffer(std::move(buffer), 0,
            Circleobject_shader::Position{},
            Circleobject_shader::Local_position{});

    shader.draw(mesh);
}