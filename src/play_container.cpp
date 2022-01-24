#include "play_container.h"

#include "beatmap_info_provider.h"
#include "render/drawable_circle.h"
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
#include <imgui.h>
#include <render/drawable_slider.h>

Play_container::Play_container(Api_manager& api_manager)
    : beatmap_container{api_manager}, replay_container{api_manager}, player{beatmap_container},
      coordinate_provider{static_cast<Magnum::Vector2>(size_manager.get_top_left()), replay_container.replay.get()},
      objects_manager{beatmap_container, replay_container, coordinate_provider, player}
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

Magnum::Vector2 Play_container::to_screen(const Magnum::Vector2 point)
{
    return point + static_cast<Magnum::Vector2>(size_manager.get_top_left());
}

void Play_container::draw()
{
    if(ImGui::Begin("Playfield")) {
        ImVec2 image_size = {static_cast<float>(size_manager.get_size().x()), static_cast<float>(size_manager.get_size().y())};
        ImVec2 pos = {(ImGui::GetWindowSize().x - image_size.x) / 2, (ImGui::GetWindowSize().y - image_size.y) / 2};
        ImGui::SetCursorPos(pos);
        playfield = generate_playfield_texture();
        ImGui::Image(static_cast<void*>(&playfield), image_size, {0, 1}, {1, 0});
    }
    ImGui::End();

    player.draw();
    beatmap_container.map_window();
    replay_container.replay_window();
}
Magnum::Vector2i Play_container::get_size()
{
    return size_manager.get_size();
}
