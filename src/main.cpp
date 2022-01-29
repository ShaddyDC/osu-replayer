#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Magnum.h>
#include <Magnum/Mesh.h>
#include <Magnum/Shaders/VertexColorGL.h>
#ifndef CORRADE_TARGET_EMSCRIPTEN
#include <Magnum/Platform/Sdl2Application.h>
#else
// #include <Magnum/Platform/EmscriptenApplication.h>   // Todo: Switch back to emscripten
#include <Magnum/Platform/Sdl2Application.h>
#endif

#include "api_manager.h"
#include "component.h"
#include "config_manager.h"
#include "notification_manager.h"
#include "play_container.h"
#include "render/coordinate_converter.h"
#include "version.h"
#include <Corrade/Utility/Arguments.h>
#include <Magnum/Timeline.h>
#include <charconv>
#include <imgui_internal.h>

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
    void setup_imgui();
    void imgui_docking() const;

    Shaders::VertexColorGL2D _shader;
    ImGuiIntegration::Context _imgui{NoCreate};
    Magnum::Timeline timer;

    Container components;
    Coordinate_holder coordinate_holder;

    Config_manager* config_manager = nullptr;
    std::unique_ptr<Api_manager> api_manager = nullptr;
    Play_container* play_container = nullptr;

    std::uint64_t score_id_to_load = 0;
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
    args.addOption("apikey").addOption("score").parse(arguments.argc, arguments.argv);

    config_manager = dynamic_cast<Config_manager*>(components.emplace_back(std::make_unique<Config_manager>()).get());

    config_manager->update_api_key(args.value("apikey"));// TODO: This isn't clean

    auto notification_manager = dynamic_cast<Notification_manager*>(components.emplace_back(std::make_unique<Notification_manager>()).get());

    api_manager = std::make_unique<Api_manager>(config_manager->config.api_key, *notification_manager);

    play_container = dynamic_cast<Play_container*>(components.emplace_back(std::make_unique<Play_container>(*api_manager)).get());

    coordinate_holder.set_resolution(play_container->get_size_manager().get_internal_size());

    setup_imgui();

#if !defined(MAGNUM_TARGET_WEBGL)
/* Have some sane speed, please */
// setMinimalLoopPeriod(16);
#endif


    if(!args.value("score").empty()) {
        auto s = args.value("score");
        std::from_chars(s.data(), s.data() + s.size(), score_id_to_load);
        Corrade::Utility::Debug() << "Loading score fully initialised" << score_id_to_load;
    }
    timer.start();
}
void TriangleExample::setup_imgui()
{
    _imgui = ImGuiIntegration::Context(Vector2{windowSize()} / dpiScaling(),
                                       windowSize(), framebufferSize());

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigWindowsResizeFromEdges = true;

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
}

void TriangleExample::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    _imgui.newFrame();

    imgui_docking();

    constexpr const auto ms_in_s = 1000;
    //Todo: Handle fractions better
    const auto time_passed = std::chrono::milliseconds{static_cast<int>(timer.previousFrameDuration() * ms_in_s)};

    if(score_id_to_load != 0 && config_manager->is_loaded()){
        Corrade::Utility::Debug() << "Loading score" << score_id_to_load;
        play_container->set_score(score_id_to_load);
        score_id_to_load = 0;
    }

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
void TriangleExample::imgui_docking() const
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("InvisibleWindow", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground);
    ImGui::PopStyleVar(3);

    ImGuiID dock_space_id = ImGui::GetID("InvisibleWindowDockSpace");

    if(!ImGui::DockBuilderGetNode(dock_space_id)) {
        ImGui::DockBuilderAddNode(dock_space_id);

        auto dock_main = dock_space_id;

        ImGuiID left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2, nullptr, &dock_main);
        ImGuiID left_bot = ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.3, nullptr, &left);
        ImGuiID bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.2, nullptr, &dock_main);

        ImGui::DockBuilderDockWindow("Playfield", dock_main);
        ImGui::DockBuilderDockWindow("Controls", bottom);
        ImGui::DockBuilderDockWindow("Beatmap", left);
        ImGui::DockBuilderDockWindow("Load Map", left_bot);
        ImGui::DockBuilderDockWindow("Load Replay", left_bot);
        ImGui::DockBuilderDockWindow("Replay", left);
        ImGui::DockBuilderDockWindow("config", left);
        ImGui::DockBuilderDockWindow("Debug", left);
        ImGui::DockBuilderDockWindow("Dear ImGui Metrics/Debugger", left);

        ImGui::DockBuilderFinish(dock_space_id);
    }

    ImGui::DockSpace(dock_space_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void TriangleExample::viewportEvent(ViewportEvent& event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Vector2{event.windowSize()} / event.dpiScaling(),
                    event.windowSize(), event.framebufferSize());
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