#include "line_renderer.h"

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Mesh.h>

void Line_renderer::draw(std::vector<Line_vert> line, const float width)
{
    Magnum::GL::Buffer buffer;

    buffer.setData(line);

    Magnum::GL::Mesh mesh;
    mesh.setCount(line.size())
        .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
        .addVertexBuffer(std::move(buffer), 0,
            Minimal_shader::Position{},
            Minimal_shader::Color{});

    shader.draw(mesh);
}
