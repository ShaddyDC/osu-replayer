#include "line_renderer.h"

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Mesh.h>

Line_renderer::Line_renderer(const Coordinate_holder& holder)
: converter { holder }
{}

void Line_renderer::draw(std::vector<Line_vert> line, const float width)
{
    for(auto& vertex : line){
        vertex.position = converter.to_screen(vertex.position);
    }

    Magnum::GL::Buffer buffer;

    buffer.setData(line);

    Magnum::GL::Mesh mesh;
    mesh.setCount(line.size())
        .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
        .addVertexBuffer(std::move(buffer), 0,
            Magnum::Shaders::VertexColor2D::Position{},
            Magnum::Shaders::VertexColor2D::Color3{});

    shader.draw(mesh);
}
