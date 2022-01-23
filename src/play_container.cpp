#include "play_container.h"

#include "beatmap_info_provider.h"
#include "render/drawable_circle.h"
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
#include <osu_reader/beatmap_util.h>
#include <render/drawable_slider.h>
#include <imgui.h>

Play_container::Play_container(Api_manager& api_manager) : data{api_manager}, replay_container{api_manager}
{
}

void Play_container::update(std::chrono::milliseconds time_passed)
{
    // Set playfield size
    if(ImGui::Begin("Playfield")) {
        const auto window_size = ImGui::GetWindowSize();
        constexpr auto bottom_offset = 20.f;// Prevent scrollbar from appearing
        const float scale = std::min((window_size.y - bottom_offset) / static_cast<float>(size_unscaled.y()),
                                     window_size.x / static_cast<float>(size_unscaled.x()));
        size_scale = scale;
    }
    ImGui::End();

    // TODO: This is doing too much

    if(paused) time_passed = std::chrono::milliseconds::zero();
    if(last_time == current_time && time_passed == std::chrono::milliseconds::zero()) return;

    // Size stuff
    scaling_size = static_cast<Magnum::Vector2i>(size_scale * size_unscaled);

    // Hitobject stuff
    current_time += duration_cast<std::chrono::milliseconds>(speed * time_passed);//Todo: Handle fractions better
    if(current_time.count() >= data.time_range().y().count()) current_time = std::chrono::milliseconds::zero();

    const Playfield_coordinate_provider coordinate_provider{static_cast<Magnum::Vector2>(top_left),
                                                            (osu::has_mods(replay_container.replay->mods, osu::Mods::HardRock))};
    const auto mods = replay_container.replay ? replay_container.replay->mods : osu::Mods{};
    const Beatmap_info_provider info_provider{*data.map, mods};

    drawables.clear();
    Drawables approach_circles{};

    const auto add_approach_circle = [this, &approach_circles, &info_provider](auto pos, auto time) {
        const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(info_provider.ar()))};
        const auto early_duration = time - current_time;
        const auto progress = static_cast<float>(early_duration.count()) / static_cast<float>(early_window.count());
        const auto radius = osu::cs_to_osupixel(info_provider.cs()) * (1.f + 2 * progress);
        approach_circles.push_back(std::make_unique<Drawable_circle>(circle_renderer, pos, radius, Circle_draw_options{.circle_center = Circleobject_shader::hollow}));
    };

    const auto draw_circle = [this, &add_approach_circle, &coordinate_provider, &info_provider](auto& circle) {
        const auto position = coordinate_provider.convert_point(circle.position);

        drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, position, osu::cs_to_osupixel(info_provider.cs()), Circle_draw_options{}));
        if(circle.time > current_time) add_approach_circle(position, circle.time);
    };

    const auto draw_slider = [this, &add_approach_circle, &coordinate_provider, &info_provider](auto& slider) {
        if(slider.time > current_time) add_approach_circle(coordinate_provider.convert_point(vector_o2m(slider.slider.points.front())), slider.time);
        drawables.push_back(std::make_unique<Drawable_slider>(slider_renderer, circle_renderer, slider, coordinate_provider,
                                                              osu::cs_to_osupixel(info_provider.cs()), current_time));
    };

    auto circles = data.circles_at(current_time, info_provider);
    auto sliders = data.sliders_at(current_time, info_provider);

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
        const auto current_frame = replay_container.frame_at(current_time);
        auto pos = Magnum::Vector2{current_frame.x, current_frame.y};
        pos = to_screen(pos);
        drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer,
                                                              pos,
                                                              osu::cs_to_osupixel(info_provider.cs()) / ratio_cursor_cs,
                                                              Circle_draw_options{
                                                                      .color = Magnum::Color4::yellow(),
                                                                      .circle_center = Circleobject_shader::filled}));
    }

    last_time = current_time;
}

Magnum::GL::Texture2D Play_container::generate_playfield_texture()
{
    // Prepare Texture
    Magnum::GL::Texture2D texture;
    texture.setStorage(1, Magnum::GL::TextureFormat::RGBA8, scaling_size);

    Magnum::GL::Framebuffer framebuffer{{{}, scaling_size}};
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
    return point + static_cast<Magnum::Vector2>(top_left);
}
void Play_container::draw()
{
    if(ImGui::Begin("Playfield")) {
        ImVec2 image_size = {static_cast<float>(scaling_size.x()), static_cast<float>(scaling_size.y())};
        ImVec2 pos = {(ImGui::GetWindowSize().x - image_size.x) / 2, (ImGui::GetWindowSize().y - image_size.y) / 2};
        ImGui::SetCursorPos(pos);
        playfield = generate_playfield_texture();
        ImGui::Image(static_cast<void*>(&playfield), image_size, {0, 1}, {1, 0});
    }
    ImGui::End();

    if(ImGui::Begin("Controls")) {
        const auto range = data.time_range();

        auto time = static_cast<int>(current_time.count());
        ImGui::SliderInt("Time", &time, static_cast<int>(range.x().count()), static_cast<int>(range.y().count()));
        current_time = std::chrono::milliseconds{time};
        ImGui::InputFloat("Speed", &speed);
        ImGui::Checkbox("Paused", &paused);
        ImGui::InputFloat("Size", &size_scale);
    }
    ImGui::End();

    data.map_window();
    replay_container.replay_window();
}
