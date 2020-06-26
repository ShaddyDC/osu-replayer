#include "slider_renderer.h"

#include <Magnum/Math/Vector2.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Mesh.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/DefaultFramebuffer.h>

struct Vertex_2d {
    Magnum::Vector2 position;
    Magnum::Vector2 textureCoordinates;
};

void Slider_renderer::draw(std::vector<Slider_vert> slider, const float radius)
{
    const auto head_pos = (slider.front().position.xy() + slider[6].position.xy()) / 2.f;

    Magnum::GL::Buffer buffer;
    buffer.setData(slider);

    Magnum::GL::Mesh mesh;
    mesh.setCount(slider.size())
        .setPrimitive(Magnum::MeshPrimitive::Triangles)
        .addVertexBuffer(std::move(buffer), 0,
            Sliderbody_shader::Position{},
            Sliderbody_shader::Side{});


    const auto size = Magnum::GL::defaultFramebuffer.viewport().size();
    Magnum::GL::Texture2D color;
    Magnum::GL::Renderbuffer depthStencil;
    color.setStorage(1, Magnum::GL::TextureFormat::RGBA8, size);
    depthStencil.setStorage(Magnum::GL::RenderbufferFormat::Depth24Stencil8, size);

    Magnum::GL::Framebuffer framebuffer{{{}, size}};
    framebuffer.attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0}, color, 0);
    framebuffer.attachRenderbuffer(
        Magnum::GL::Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    framebuffer.clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth)
        .bind();
    
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    circle_renderer.draw(head_pos, radius);
    shader.draw(mesh);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);

    Vertex_2d verts[] = {
        {{-1.f, -1.f}, {0.f, 0.f}},
        {{-1.f, 1.f}, {0.f, 1.f}},
        {{1.f, -1.f}, {1.f, 0.f}},
        {{1.f, 1.f}, {1.f, 1.f}},
    };
    Magnum::GL::Buffer verts_tmp;
    verts_tmp.setData(verts);
    Magnum::GL::Mesh mesh_tmp;
    mesh_tmp.setCount(4)
        .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
        .addVertexBuffer(std::move(verts_tmp), 0,
            Magnum::Shaders::Flat2D::Position{},
            Magnum::Shaders::Flat2D::TextureCoordinates{});

    // Render rest
    Magnum::GL::defaultFramebuffer.bind();
    flat_shader.bindTexture(color).draw(mesh_tmp);
}
