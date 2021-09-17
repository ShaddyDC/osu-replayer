#include "circleobject_renderer.h"
#include <Magnum/Math/Color.h>
#include <Magnum/Mesh.h>

struct Circle_vertex {
    Magnum::Vector2 position;
    Magnum::Vector2 local_position;
};

Circleobject_mesh Circleobject_renderer::generate_mesh(const Magnum::Vector2 position, const float radius)
{
    Circle_vertex data[] = {
            {position + Magnum::Vector2{-radius, -radius}, {-1, -1}},
            {position + Magnum::Vector2{-radius, radius}, {-1, 1}},
            {position + Magnum::Vector2{radius, -radius}, {1, -1}},
            {position + Magnum::Vector2{radius, radius}, {1, 1}},
    };

    Magnum::GL::Buffer buffer{};
    buffer.setData(data);
    Magnum::GL::Mesh mesh;
    mesh.setCount(4)
            .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
            .addVertexBuffer(std::move(buffer), 0,
                             Circleobject_shader::Position{},
                             Circleobject_shader::Local_position{});

    return mesh;
}

void Circleobject_renderer::draw(Circleobject_mesh& mesh, Circle_draw_options options)
{
    shader.set_color(options.color);
    shader.set_circle_center(options.circle_center);
    shader.draw(mesh);
}