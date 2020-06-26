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

Slider_mesh Slider_renderer::generate_mesh(const Slider& slider, const float radius)
{
    Slider_mesh mesh;

    mesh.head = circle_renderer.generate_mesh(slider.front().front(), radius);

    const auto flatten_slider = [](const Slider& slider){
        Slider_segment out = slider.front();
        for(std::size_t i = 1; i < slider.size(); ++i){
            std::copy(slider[i].begin() + 1, slider[i].end(), std::back_inserter(out));
        }
        return out;
    };
    const auto slider_verts = vertex_generate(flatten_slider(slider));
    
    Magnum::GL::Buffer buffer;
    buffer.setData(slider_verts);

    mesh.body.setCount(slider_verts.size())
        .setPrimitive(Magnum::MeshPrimitive::Triangles)
        .addVertexBuffer(std::move(buffer), 0,
            Sliderbody_shader::Position{},
            Sliderbody_shader::Side{});

    return mesh;
}

Magnum::GL::Texture2D create_texture(
    Slider_renderer& slider_renderer, Circleobject_renderer& circle_renderer, Slider_mesh& mesh)
{
    Magnum::GL::Texture2D texture;
    const auto size = Magnum::GL::defaultFramebuffer.viewport().size();
    Magnum::GL::Renderbuffer depthStencil;
    texture.setStorage(1, Magnum::GL::TextureFormat::RGBA8, size);
    depthStencil.setStorage(Magnum::GL::RenderbufferFormat::Depth24Stencil8, size);

    Magnum::GL::Framebuffer framebuffer{{{}, size}};
    framebuffer.attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0}, texture, 0);
    framebuffer.attachRenderbuffer(
        Magnum::GL::Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    framebuffer.clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth)
        .bind();
    
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    circle_renderer.draw(mesh.head);
    slider_renderer.draw(mesh.body);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);

    return texture;
}

void Slider_renderer::draw(Slider_mesh& mesh)
{
    auto texture = create_texture(*this, circle_renderer, mesh);

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
    flat_shader.bindTexture(texture).draw(mesh_tmp);
}

void Slider_renderer::draw(Sliderbody_mesh& mesh)
{
    shader.draw(mesh);
}
