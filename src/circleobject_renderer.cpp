#include "circleobject_renderer.h"
#include <Magnum/GL/Buffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/GL/Mesh.h>

struct Circle_vertex{
    Magnum::Vector2 position;
    Magnum::Vector2 local_position;
};

Circleobject_renderer::Circleobject_renderer(const Coordinate_holder& holder)
: converter { holder }
{}

void Circleobject_renderer::draw(const Magnum::Vector2 position, const float radius)
{
    Circle_vertex data[] = {
        { position + Magnum::Vector2{ -radius, -radius }, { -1, -1 }},
        { position + Magnum::Vector2{ -radius, radius }, { -1, 1 }},
        { position + Magnum::Vector2{ radius, -radius }, { 1, -1 }},
        { position + Magnum::Vector2{ radius, radius }, { 1, 1 }},
    };
    for(auto& vertex : data){
        vertex.position = converter.to_screen(vertex.position);
    }

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