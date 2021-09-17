#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Mesh.h>
#include <Magnum/Shaders/VertexColorGL.h>
#ifndef CORRADE_TARGET_EMSCRIPTEN
#include <Magnum/Platform/Sdl2Application.h>
#else
// #include <Magnum/Platform/EmscriptenApplication.h>   // Todo: Switch back to emscripten
#include <Magnum/Platform/Sdl2Application.h>
#endif

#include "config_manager.h"
#include "play_container.h"
#include "render/coordinate_converter.h"

#include <Corrade/Utility/Arguments.h>
#include <Magnum/Timeline.h>

#include "api_manager.h"
#include "version.h"

using namespace Magnum;
using namespace Math::Literals;

class TriangleExample : public Platform::Application {
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
    Shaders::VertexColorGL2D _shader;

    ImGuiIntegration::Context _imgui{NoCreate};

    Magnum::Timeline timer;
    Play_container play_container;

    Magnum::GL::Texture2D playtext;

    Config_manager config_manager;
    Api_manager api_manager{config_manager.config.api_key};
};

TriangleExample::TriangleExample(const Arguments& arguments) : Platform::Application{arguments,
                                                                                     Configuration{}
                                                                                             .setTitle("Magnum Triangle Example")
                                                                                             .setSize({1600, 900})
                                                                                             .setWindowFlags(Configuration::WindowFlag::Resizable)},
                                                               play_container{api_manager}
{
    using namespace Math::Literals;

    Magnum::Debug() << "Using Commit " << Version::git_commit_hash;
    Magnum::Debug() << "Commit from " << Version::git_commit_time;
    Magnum::Debug() << "Built at " << Version::build_time;

    Corrade::Utility::Arguments args{"replayer"};
    args.addOption("apikey").parse(arguments.argc, arguments.argv);
    config_manager.update_api_key(args.value("apikey"));

    coordinate_holder.set_resolution(play_container.size);

    struct TriangleVertex {
        Vector2 position;
        Color3 color;
    };

    const TriangleVertex data[]{
            {{-0.5f, -0.5f}, 0xff0000_rgbf}, /* Left vertex, red color */
            {{0.5f, -0.5f}, 0x00ff00_rgbf},  /* Right vertex, green color */
            {{0.0f, 0.5f}, 0x0000ff_rgbf}    /* Top vertex, blue color */
    };

    GL::Buffer buffer;
    buffer.setData(data);

    _mesh.setCount(3)
            .addVertexBuffer(std::move(buffer), 0,
                             Shaders::VertexColorGL2D::Position{},
                             Shaders::VertexColorGL2D::Color3{});

    _imgui = ImGuiIntegration::Context(Vector2{windowSize()} / dpiScaling(),
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

void TriangleExample::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    _imgui.newFrame();

    _shader.draw(_mesh);

    // Set playfield size
    if(ImGui::Begin("Playfield")) {
        const auto size = ImGui::GetWindowSize();
        constexpr auto bottom_offset = 20.f;// Prevent scrollbar from appearing
        const float scale = std::min((size.y - bottom_offset) / play_container.size.y(), size.x / play_container.size.x());
        play_container.size_scale = scale;
    }
    ImGui::End();

    play_container.update(std::chrono::milliseconds{static_cast<int>(timer.previousFrameDuration() * 1000.f)});//Todo: Handle fractions better

    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    if(ImGui::Begin("Playfield")) {
        ImVec2 image_size = {static_cast<float>(play_container.scaling_size.x()), static_cast<float>(play_container.scaling_size.y())};
        ImVec2 pos = {(ImGui::GetWindowSize().x - image_size.x) / 2, (ImGui::GetWindowSize().y - image_size.y) / 2};
        ImGui::SetCursorPos(pos);
        playtext = play_container.draw();
        ImGui::Image(static_cast<void*>(&playtext), image_size, {0, 1}, {1, 0});
        // ImGui::GetWindowDrawList()->AddImage
    }
    ImGui::End();

    if(ImGui::Begin("Controls")) {
        const auto range = play_container.data.time_range();

        int time = play_container.current_time.count();
        ImGui::SliderInt("Time", &time, range.x().count(), range.y().count());
        play_container.current_time = std::chrono::milliseconds{time};
        ImGui::InputFloat("Speed", &play_container.speed);
        ImGui::Checkbox("Paused", &play_container.paused);
        ImGui::InputFloat("Size", &play_container.size_scale);
    }
    ImGui::End();

    play_container.data.map_window();
    play_container.replay_container.replay_window();
    config_manager.config_window();

    ImGui::ShowMetricsWindow();

    /* Update application cursor */
    _imgui.updateApplicationCursor(*this);

    _imgui.drawFrame();

    swapBuffers();
    redraw();
    timer.nextFrame();
}

void TriangleExample::viewportEvent(ViewportEvent& event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Vector2{event.windowSize()} / event.dpiScaling(),
                    event.windowSize(), event.framebufferSize());

    coordinate_holder.set_resolution(play_container.size);//Todo: Rework class
}

void TriangleExample::keyPressEvent(KeyEvent& event)
{
    if(_imgui.handleKeyPressEvent(event)) return;
}

void TriangleExample::keyReleaseEvent(KeyEvent& event)
{
    if(_imgui.handleKeyReleaseEvent(event)) return;
}

void TriangleExample::mousePressEvent(MouseEvent& event)
{
    if(_imgui.handleMousePressEvent(event)) return;
}

void TriangleExample::mouseReleaseEvent(MouseEvent& event)
{
    if(_imgui.handleMouseReleaseEvent(event)) return;
}

void TriangleExample::mouseMoveEvent(MouseMoveEvent& event)
{
    if(_imgui.handleMouseMoveEvent(event)) return;
}

void TriangleExample::mouseScrollEvent(MouseScrollEvent& event)
{
    if(_imgui.handleMouseScrollEvent(event)) {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }
}

void TriangleExample::textInputEvent(TextInputEvent& event)
{
    if(_imgui.handleTextInputEvent(event)) return;
}

MAGNUM_APPLICATION_MAIN(TriangleExample)