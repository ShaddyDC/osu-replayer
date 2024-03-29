#include "line_renderer.h"

#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Mesh.h>

Line_mesh Line_renderer::generate_mesh(const std::vector<Magnum::Vector2>& line, const float width, const std::vector<Magnum::Color4>& colors)
{
    const auto line_verts = line_generate(line, width, colors);

    Magnum::GL::Buffer buffer;
    buffer.setData(line_verts);

    const auto primitive = colors.size() < line.size() - 1
                                   ? Magnum::MeshPrimitive::TriangleStrip
                                   : Magnum::MeshPrimitive::Triangles;

    Magnum::GL::Mesh mesh;
    mesh.setCount(static_cast<int>(line_verts.size()))
            .setPrimitive(primitive)
            .addVertexBuffer(std::move(buffer), 0,
                             Minimal_shader::Position{},
                             Minimal_shader::Color{});

    return mesh;
}

void Line_renderer::draw(Line_mesh& mesh)
{
    shader.draw(mesh);
}
