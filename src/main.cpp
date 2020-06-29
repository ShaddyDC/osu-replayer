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
#include "render/shaders/sliderbody_shader.h"
#include "render/coordinate_converter.h"
#include "play_container.h"

#include <Magnum/Timeline.h>

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

        GL::Mesh _mesh;
        Shaders::VertexColor2D _shader;

        ImGuiIntegration::Context _imgui{NoCreate};

        Magnum::Timeline timer;
        Play_container play_container;

        Magnum::GL::Texture2D playtext;
};

TriangleExample::TriangleExample(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}.setTitle("Magnum Triangle Example").setSize({1600, 900})}
{
    using namespace Math::Literals;

    coordinate_holder.set_resolution(play_container.size);

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
    // setMinimalLoopPeriod(16);
    #endif

    timer.start();
}

void TriangleExample::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    _shader.draw(_mesh);

    play_container.update(static_cast<int>(timer.previousFrameDuration() * 1000.f));    //Todo: Handle fractions better

   _imgui.newFrame();

    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    if(ImGui::Begin("Playfield")){
        playtext = play_container.draw();
        ImGui::Image((void*)&playtext, { (float)play_container.scaling_size.x(), (float)play_container.scaling_size.y() }, {0, 1}, {1, 0});
        // ImGui::GetWindowDrawList()->AddImage
        ImGui::End();
    }

    ImGui::Begin("Controls");
    // ImGui::InputText("Slider", slider_string.data(), slider_string.size());
    const auto range = play_container.data.time_range();
    ImGui::SliderInt("Time", &play_container.current_time, range.x(), range.y());
    ImGui::InputFloat("Speed", &play_container.speed);
    ImGui::Checkbox("Paused", &play_container.paused);
    ImGui::InputFloat("Size", &play_container.size_scale);

    ImGui::End();

    ImGui::ShowMetricsWindow();

    /* Update application cursor */
    _imgui.updateApplicationCursor(*this);

    _imgui.drawFrame();

    swapBuffers();
    redraw();
    timer.nextFrame();
}

void TriangleExample::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
        event.windowSize(), event.framebufferSize());

    coordinate_holder.set_resolution(play_container.size);  //Todo: Rework class
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