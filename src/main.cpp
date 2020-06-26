#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Mesh.h>
#ifndef CORRADE_TARGET_EMSCRIPTEN
#include <Magnum/Platform/Sdl2Application.h>
#else
#include <Magnum/Platform/EmscriptenApplication.h>
#endif

#include "slider.h"
#include "vertex_generate.h"
#include "shaders/sliderbody_shader.h"
#include "shaders/circleobject_shader.h"

#include <Magnum/Math/Matrix4.h>
#include <Corrade/Utility/Resource.h>
#include <Corrade/Utility/DebugStl.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Shaders/Flat.h>

using namespace Magnum;
using namespace Math::Literals;

class TriangleExample: public Platform::Application {
    public:
        explicit TriangleExample(const Arguments& arguments);
        void drawEvent() override;


        void viewportEvent(ViewportEvent& event) override;

        void keyPressEvent(KeyEvent& event) override;
        void keyReleaseEvent(KeyEvent& event) override;

        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;
        void textInputEvent(TextInputEvent& event) override;
    private:

        GL::Mesh _mesh;
        Shaders::VertexColor2D _shader;

        GL::Mesh slider_mesh;
        Sliderbody_shader slider_shader;

        GL::Mesh head_mesh;
        Circleobject_shader head_shader;

        ImGuiIntegration::Context _imgui{NoCreate};

        Shaders::Flat2D flat_shader{Shaders::Flat2D::Flag::Textured};

        std::vector<char> slider_string;
        Slider slider;
        std::vector<Slider_vert> slider_verts;
        std::vector<Line_vert> line_verts;
};

TriangleExample::TriangleExample(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}.setTitle("Magnum Triangle Example").setSize({1600, 900})}
{
    using namespace Math::Literals;

    struct TriangleVertex {
        Vector2 position;
        Color3 color;
    };
    const TriangleVertex data[]{
        {{-0.5f, -0.5f}, 0xff0000_rgbf},    /* Left vertex, red color */
        {{ 0.5f, -0.5f}, 0x00ff00_rgbf},    /* Right vertex, green color */
        {{ 0.0f,  0.5f}, 0x0000ff_rgbf}     /* Top vertex, blue color */
    };

    GL::Buffer buffer;
    buffer.setData(data);

    _mesh.setCount(3)
         .addVertexBuffer(std::move(buffer), 0,
            Shaders::VertexColor2D::Position{},
            Shaders::VertexColor2D::Color3{});

    slider_mesh.setCount(0);
    head_mesh.setCount(0);


    _imgui = ImGuiIntegration::Context(Vector2{windowSize()}/dpiScaling(),
        windowSize(), framebufferSize());

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
        GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);

    #if !defined(MAGNUM_TARGET_WEBGL)
    /* Have some sane speed, please */
    setMinimalLoopPeriod(16);
    #endif

    std::string_view s = "100,100,12600,6,1,B|200:200|250:200|250:200|300:150,2,310.123,2|1|2,0:0|0:0|0:2,0:0:0:0:";
    slider_string = { s.begin(), s.end() };
    slider_string.resize(256);
}

void TriangleExample::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    // Render Slider
    const Vector2i size = GL::defaultFramebuffer.viewport().size();
    GL::Texture2D color;
    GL::Renderbuffer depthStencil;
    color.setStorage(1, GL::TextureFormat::RGBA8, size);
    depthStencil.setStorage(GL::RenderbufferFormat::Depth24Stencil8, size);

    GL::Framebuffer framebuffer{{{}, size}};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, color, 0);
    framebuffer.attachRenderbuffer(
        GL::Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    framebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth)
        .bind();
    
    GL::Renderer::disable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    head_shader.draw(head_mesh);
    slider_shader.draw(slider_mesh);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::Blending);

    struct Vertex {
        Vector2 position;
        Vector2 textureCoordinates;
    };
    Vertex verts[] = {
        {{-1.f, -1.f}, {0.f, 0.f}},
        {{-1.f, 1.f}, {0.f, 1.f}},
        {{1.f, -1.f}, {1.f, 0.f}},
        {{1.f, 1.f}, {1.f, 1.f}},
    };
    GL::Buffer verts_tmp;
    verts_tmp.setData(verts);
    GL::Mesh mesh_tmp;
    mesh_tmp.setCount(4)
        .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
        .addVertexBuffer(std::move(verts_tmp), 0,
            Shaders::Flat2D::Position{},
            Shaders::Flat2D::TextureCoordinates{});

    // Render rest
    GL::defaultFramebuffer.bind();
    _shader.draw(_mesh);
    flat_shader.bindTexture(color).draw(mesh_tmp);
    

   _imgui.newFrame();

    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    ImGui::Begin("Slider");
    ImGui::InputText("Slider", slider_string.data(), slider_string.size());
    if(ImGui::Button("Apply")){
        printf("Prasing slider: %s\n", slider_string.data());
        slider = parse_slider(slider_string.data()).value();

        for(const auto& segment : slider){
            printf("Segment\n");
            for(const auto point : segment){
                printf("x: %f y: %f\n", point.x(), point.y());
            }
        }

        const auto flatten_slider = [](const Slider& slider){
            Slider_segment out = slider.front();
            for(std::size_t i = 1; i < slider.size(); ++i){
                std::copy(slider[i].begin() + 1, slider[i].end(), std::back_inserter(out));
            }
            return out;
        };
        slider_verts = vertex_generate(flatten_slider(slider));

        std::vector<Magnum::Color3> colors{
            Magnum::Color3::red(),
            Magnum::Color3::blue(),
            Magnum::Color3::green(),
            Magnum::Color3::yellow(),
            // Magnum::Color3::magenta(),
        };

        line_verts = line_generate(flatten_slider(slider), 5.f, colors);

        const auto x = (Magnum::Vector2)windowSize();
        auto m = Matrix4::perspectiveProjection({x.x(), 0}, {0,x.y()}, -1.f, 1.f);
        for(auto& v : slider_verts){
            const auto old_col = v.position.z();
            v.position = (m * Magnum::Vector4{v.position.x(), v.position.y(), 1, 0}).xyz();
            v.position.z() = old_col;
        }
        for(auto& v : line_verts){
            v.position = (m * Magnum::Vector4{Magnum::Vector3{v.position, 1}, {}}).xy();
        }

        GL::Buffer buffer;

        buffer.setData(slider_verts);
        slider_mesh.setCount(slider_verts.size())
            .setPrimitive(Magnum::MeshPrimitive::Triangles)
            .addVertexBuffer(std::move(buffer), 0,
                Sliderbody_shader::Position{},
                Sliderbody_shader::Side{});

        buffer = GL::Buffer{};

        buffer.setData(line_verts);
        _mesh.setCount(line_verts.size())
            .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
            .addVertexBuffer(std::move(buffer), 0,
                Shaders::VertexColor2D::Position{},
                Shaders::VertexColor2D::Color3{});

        // head
        buffer = GL::Buffer{};
        struct CVerts{
            Magnum::Vector2 pos;
            Magnum::Vector2 center;
        };
        const auto center = slider.front().front();
        CVerts d[] = {
            {center + Magnum::Vector2{-30, -30}, {-1, -1}},
            {center + Magnum::Vector2{-30, 30}, {-1, 1}},
            {center + Magnum::Vector2{30, -30}, {1, -1}},
            {center + Magnum::Vector2{30, 30}, {1, 1}},
        };
        for(auto& v : d){
            v.pos = (m * Magnum::Vector4{Magnum::Vector3{v.pos, 1}, {}}).xy();
        }
        buffer.setData(d);
        head_mesh.setCount(4)
            .setPrimitive(Magnum::MeshPrimitive::TriangleStrip)
            .addVertexBuffer(std::move(buffer), 0,
                Circleobject_shader::Position{},
                Circleobject_shader::Local_position{});
    }
    ImGui::End();

    /* Update application cursor */
    _imgui.updateApplicationCursor(*this);

    _imgui.drawFrame();

    swapBuffers();
    redraw();
}

void TriangleExample::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
        event.windowSize(), event.framebufferSize());
}

void TriangleExample::keyPressEvent(KeyEvent& event) {
    if(_imgui.handleKeyPressEvent(event)) return;
}

void TriangleExample::keyReleaseEvent(KeyEvent& event) {
    if(_imgui.handleKeyReleaseEvent(event)) return;
}

void TriangleExample::mousePressEvent(MouseEvent& event) {
    if(_imgui.handleMousePressEvent(event)) return;
}

void TriangleExample::mouseReleaseEvent(MouseEvent& event) {
    if(_imgui.handleMouseReleaseEvent(event)) return;
}

void TriangleExample::mouseMoveEvent(MouseMoveEvent& event) {
    if(_imgui.handleMouseMoveEvent(event)) return;
}

void TriangleExample::mouseScrollEvent(MouseScrollEvent& event) {
    if(_imgui.handleMouseScrollEvent(event)) {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }
}

void TriangleExample::textInputEvent(TextInputEvent& event) {
    if(_imgui.handleTextInputEvent(event)) return;
}

MAGNUM_APPLICATION_MAIN(TriangleExample)