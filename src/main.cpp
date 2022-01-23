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
#include "component.h"
#include "notification_manager.h"
#include "version.h"

using namespace Magnum;
using namespace Math::Literals;

constexpr const auto default_window_height = 900;
constexpr const auto default_window_width = 1600;

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
    Container components;
    Coordinate_holder coordinate_holder;

    Shaders::VertexColorGL2D _shader;

    ImGuiIntegration::Context _imgui{NoCreate};

    Magnum::Timeline timer;

    Magnum::GL::Texture2D playtext;

    Config_manager* config_manager = nullptr;
    Notification_manager* notification_manager = nullptr;
    Api_manager api_manager{config_manager->config.api_key};
    Play_container* play_container = nullptr;
};

TriangleExample::TriangleExample(const Arguments& arguments) : Platform::Application{arguments,
                                                                                     Configuration{}
                                                                                             .setTitle("Magnum Triangle Example")
                                                                                             .setSize({default_window_width, default_window_height})
                                                                                             .setWindowFlags(Configuration::WindowFlag::Resizable)}
{
    using namespace Math::Literals;

    Magnum::Debug() << "Using Commit " << Version::git_commit_hash;
    Magnum::Debug() << "Commit from " << Version::git_commit_time;
    Magnum::Debug() << "Built at " << Version::build_time;

    Corrade::Utility::Arguments args{"replayer"};
    args.addOption("apikey").parse(arguments.argc, arguments.argv);

    config_manager = dynamic_cast<Config_manager*>(components.emplace_back(std::make_unique<Config_manager>()).get());

    config_manager->update_api_key(args.value("apikey")); // TODO: This isn't clean

    components.emplace_back(std::make_unique<Notification_manager>());
    play_container = dynamic_cast<Play_container*>(components.emplace_back(std::make_unique<Play_container>(api_manager)).get());

    coordinate_holder.set_resolution(play_container->get_size());

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

    constexpr const auto ms_in_s = 1000;
    //Todo: Handle fractions better
    const auto time_passed = std::chrono::milliseconds{static_cast<int>(timer.previousFrameDuration() * ms_in_s)};

    for(auto& component : components) {
        component->update(time_passed);
    }

    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    for(auto& component : components) {
        component->draw();
    }

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

    coordinate_holder.set_resolution(play_container->get_size());
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