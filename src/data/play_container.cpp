#include "play_container.h"

#include "Magnum/ImGuiIntegration/Integration.h"
#include "beatmap_info_provider.h"
#include "render/drawable_circle.h"
#include "render/drawable_slider.h"
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
#include <imgui.h>

Play_container::Play_container(Bindable<std::optional<osu::Beatmap>>& bm, Bindable<std::optional<osu::Replay>>& rp)
    : beatmap{bm}, replay{rp},
      player{beatmap},
      coordinate_provider{static_cast<Magnum::Vector2>(size_manager.get_top_left()), rp.get()},
      objects_manager{beatmap, replay, coordinate_provider, player}
{
}

void Play_container::update(std::chrono::milliseconds time_passed)
{
    player.update(time_passed);
    size_manager.update(time_passed);
    coordinate_provider.update(time_passed);
    objects_manager.update(time_passed);
}

Magnum::GL::Texture2D Play_container::generate_playfield_texture()
{
    // Prepare Texture and fresh framebuffer
    Magnum::GL::Texture2D texture;
    texture.setStorage(1, Magnum::GL::TextureFormat::RGBA8, size_manager.get_size());

    Magnum::GL::Framebuffer framebuffer{{{}, size_manager.get_size()}};
    framebuffer.bind();
    framebuffer.attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0}, texture, 0);

    framebuffer.clearColor(0, Magnum::Color4{0, 0, 0, 0});
    framebuffer.clear(Magnum::GL::FramebufferClear::Depth).bind();

    //Todo: Better to set viewport accordingly
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);

    // Actual drawing
    border.draw(framebuffer);
    objects_manager.draw(framebuffer);

    // Reset stuff
    Magnum::GL::defaultFramebuffer.bind();
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);

    return texture;
}

void Play_container::draw_playfield()
{
    ImGui::BeginChild("playfield", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_NoMove);

    ImVec2 image_size = {static_cast<float>(size_manager.get_size().x()), static_cast<float>(size_manager.get_size().y())};
    ImVec2 local_origin = {(ImGui::GetWindowSize().x - image_size.x) / 2, (ImGui::GetWindowSize().y - image_size.y) / 2};
    ImGui::SetCursorPos(local_origin);
    const auto global_origin = static_cast<Magnum::Vector2>(ImGui::GetCursorScreenPos());

    playfield = generate_playfield_texture();
    ImGui::Image(static_cast<void*>(&playfield), image_size, {0, 1}, {1, 0});

    if(ImGui::IsItemClicked()) {
        const auto pos = (static_cast<Magnum::Vector2>(ImGui::GetMousePos()) - global_origin) / size_manager.get_scale();
        Magnum::Debug() << "click" << pos << coordinate_provider.field_to_osu(pos);
    }
    ImGui::EndChild();
}

void Play_container::draw()
{
    if(ImGui::Begin("Playfield")) {
        draw_playfield();
    }
    ImGui::End();

    player.draw();
}
const Playfield_size_manager& Play_container::get_size_manager() const
{
    return size_manager;
}
