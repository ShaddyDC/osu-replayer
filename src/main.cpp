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
#include "render/vertex_generate.h"
#include "render/shaders/sliderbody_shader.h"
#include "render/coordinate_converter.h"
#include "render/circleobject_renderer.h"
#include "render/slider_renderer.h"
#include "render/line_renderer.h"

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
        Coordinate_holder coordinate_holder;
        Circleobject_renderer circle_renderer;
        Slider_renderer slider_renderer;
        Line_renderer line_renderer;

        GL::Mesh _mesh;
        Shaders::VertexColor2D _shader;

        ImGuiIntegration::Context _imgui{NoCreate};

        std::vector<char> slider_string;
        Slider slider;
        Slider_mesh slider_mesh;
        Line_mesh line_mesh;
        Line_mesh line_mesh2;
        bool loaded = false;
};

TriangleExample::TriangleExample(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}.setTitle("Magnum Triangle Example").setSize({1600, 900})}
{
    using namespace Math::Literals;

    coordinate_holder.set_resolution(windowSize());

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
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);

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

    _shader.draw(_mesh);
    if(loaded){
        line_renderer.draw(line_mesh);
        line_renderer.draw(line_mesh2);
        slider_renderer.draw(slider_mesh);
    }

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

        const auto flatten_slider = [](const Slider& slider){
            Slider_segment out = slider.front();
            for(std::size_t i = 1; i < slider.size(); ++i){
                std::copy(slider[i].begin() + 1, slider[i].end(), std::back_inserter(out));
            }
            return out;
        };

        std::vector<Magnum::Color4> colors{
            Magnum::Color4::red(),
            Magnum::Color4::blue(),
            Magnum::Color4::green(),
            Magnum::Color4::yellow(),
            Magnum::Color4::magenta(),
        };

        const auto line = flatten_slider(slider);
        Slider_segment line2{ {300, 1}, {1, 300}, {300, 300} };

        slider_mesh = slider_renderer.generate_mesh(slider, 30.f);
        line_mesh = line_renderer.generate_mesh(line, 5.f, colors);
        line_mesh2 = line_renderer.generate_mesh(line2, 2.0f, { Magnum::Color4::cyan() });
        loaded = true;
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

    coordinate_holder.set_resolution(windowSize());
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