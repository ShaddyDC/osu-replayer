#include "slider_renderer.h"

#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Mesh.h>

struct Vertex_2d {
    Magnum::Vector2 position;
    Magnum::Vector2 textureCoordinates;
};

static inline Magnum::GL::Texture2D create_texture(
        Slider_renderer& slider_renderer, Circleobject_renderer& circle_renderer, Magnum::GL::Mesh& body, Magnum::GL::Mesh& head)
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

    framebuffer.clearColor(0, Magnum::Color4{0, 0, 0, 0});
    framebuffer.clear(Magnum::GL::FramebufferClear::Depth).bind();

    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    circle_renderer.draw(head);
    slider_renderer.draw(body);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);

    return texture;
}

Magnum::GL::Texture2D Slider_renderer::generate_texture(const osu::Slider& slider, const float radius)
{
    std::vector<Magnum::Vector2> points;
    std::transform(slider.points.cbegin(), slider.points.cend(), std::back_inserter(points), [](const auto p) { return Magnum::Vector2{p.x, p.y}; });
    auto head = circle_renderer.generate_mesh(points.front(), radius);

    const auto slider_verts = vertex_generate(points, radius);

    Magnum::GL::Buffer buffer;
    buffer.setData(slider_verts);

    Magnum::GL::Mesh body;
    body.setCount(static_cast<int>(slider_verts.size()))
            .setPrimitive(Magnum::MeshPrimitive::Triangles)
            .addVertexBuffer(std::move(buffer), 0,
                             Sliderbody_shader::Position{},
                             Sliderbody_shader::Side{});


    return create_texture(*this, circle_renderer, body, head);
}

void Slider_renderer::draw(Magnum::GL::Texture2D& texture, Magnum::GL::Framebuffer& target)
{
    static const constexpr Vertex_2d verts[] = {
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
                             Magnum::Shaders::FlatGL2D::Position{},
                             Magnum::Shaders::FlatGL2D::TextureCoordinates{});

    // Render rest
    target.bind();
    flat_shader.bindTexture(texture).draw(mesh_tmp);
}

void Slider_renderer::draw(Sliderbody_mesh& mesh)
{
    shader.draw(mesh);
}
