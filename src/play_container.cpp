#include "play_container.h"

#include "beatmap_info_provider.h"
#include "render/drawable_circle.h"
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
#include <imgui.h>
#include <osu_reader/beatmap_util.h>
#include <render/drawable_slider.h>

Play_container::Play_container(Api_manager& api_manager)
    : data{api_manager}, replay_container{api_manager}, player{data},
      coordinate_provider{static_cast<Magnum::Vector2>(size_manager.get_top_left()), replay_container.replay}
{
}

void Play_container::update(std::chrono::milliseconds time_passed)
{
    player.update(time_passed);
    size_manager.update(time_passed);
    coordinate_provider.update(time_passed);

    const auto mods = replay_container.replay ? replay_container.replay->mods : osu::Mods{};
    const Beatmap_info_provider info_provider{*data.map, mods};

    drawables.clear();
    Drawables approach_circles{};

    const auto add_approach_circle = [this, &approach_circles, &info_provider](auto pos, auto time) {
        const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(info_provider.ar()))};
        const auto early_duration = time - player.get_current_time();
        const auto progress = static_cast<float>(early_duration.count()) / static_cast<float>(early_window.count());
        const auto radius = osu::cs_to_osupixel(info_provider.cs()) * (1.f + 2 * progress);
        approach_circles.push_back(std::make_unique<Drawable_circle>(circle_renderer, pos, radius, Circle_draw_options{.circle_center = Circleobject_shader::hollow}));
    };

    const auto draw_circle = [this, &add_approach_circle, &info_provider](auto& circle) {
        const auto position = coordinate_provider.convert_point(circle.position);

        drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, position, osu::cs_to_osupixel(info_provider.cs()), Circle_draw_options{}));
        if(circle.time > player.get_current_time()) add_approach_circle(position, circle.time);
    };

    const auto draw_slider = [this, &add_approach_circle, &info_provider](auto& slider) {
        if(slider.time > player.get_current_time()) add_approach_circle(coordinate_provider.convert_point(vector_o2m(slider.slider.points.front())), slider.time);
        drawables.push_back(std::make_unique<Drawable_slider>(slider_renderer, circle_renderer, slider, coordinate_provider,
                                                              osu::cs_to_osupixel(info_provider.cs()), player.get_current_time()));
    };

    auto circles = data.circles_at(player.get_current_time(), info_provider);
    auto sliders = data.sliders_at(player.get_current_time(), info_provider);

    auto circles_it = circles.begin();
    auto sliders_it = sliders.begin();

    while(circles_it != circles.end() || sliders_it != sliders.end()) {
        if(circles_it == circles.end() || (sliders_it != sliders.end() && circles_it->time >= sliders_it->time)) draw_slider(*sliders_it++);
        else
            draw_circle(*circles_it++);
    }

    std::reverse(drawables.begin(), drawables.end());
    drawables.insert(drawables.end(), std::make_move_iterator(approach_circles.begin()), std::make_move_iterator(approach_circles.end()));

    if(replay_container.replay) {
        constexpr const auto ratio_cursor_cs = 5;
        const auto current_frame = replay_container.frame_at(player.get_current_time());
        auto pos = Magnum::Vector2{current_frame.x, current_frame.y};
        pos = to_screen(pos);
        drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer,
                                                              pos,
                                                              osu::cs_to_osupixel(info_provider.cs()) / ratio_cursor_cs,
                                                              Circle_draw_options{
                                                                      .color = Magnum::Color4::yellow(),
                                                                      .circle_center = Circleobject_shader::filled}));
    }
}

Magnum::GL::Texture2D Play_container::generate_playfield_texture()
{
    // Prepare Texture
    Magnum::GL::Texture2D texture;
    texture.setStorage(1, Magnum::GL::TextureFormat::RGBA8, size_manager.get_size());

    Magnum::GL::Framebuffer framebuffer{{{}, size_manager.get_size()}};
    framebuffer.bind();
    framebuffer.attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0}, texture, 0);

    framebuffer.clearColor(0, Magnum::Color4{0, 0, 0, 0});
    framebuffer.clear(Magnum::GL::FramebufferClear::Depth).bind();

    //Todo: Better to set viewport accordingly
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);

    border.draw(framebuffer);

    for(auto& drawable : drawables) drawable->draw(framebuffer);

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
    data.map_window();
    replay_container.replay_window();
}
Magnum::Vector2i Play_container::get_size()
{
    return size_manager.get_size();
}
