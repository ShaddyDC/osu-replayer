#include "visible_hitobjects_manager.h"
#include "beatmap_info_provider.h"
#include "osu_reader/beatmap_util.h"
#include "render/drawable_circle.h"
#include "render/drawable_slider.h"

void Visible_hitobjects_manager::update(std::chrono::milliseconds /*time_passed*/)
{
    if(!beatmap.map) return;

    drawables.clear();

    const auto mods = replay.replay ? replay.replay->mods : osu::Mods{};
    const Beatmap_info_provider info_provider{*beatmap.map, mods};

    auto circles = beatmap.circles_at(player.get_current_time(), info_provider);
    auto sliders = beatmap.sliders_at(player.get_current_time(), info_provider);

    auto circles_it = circles.begin();
    auto sliders_it = sliders.begin();

    while(circles_it != circles.end() || sliders_it != sliders.end()) {
        if(circles_it == circles.end() || (sliders_it != sliders.end() && circles_it->time >= sliders_it->time)) draw_slider(*sliders_it++, info_provider);
        else
            draw_circle(*circles_it++, info_provider);
    }

    std::reverse(drawables.begin(), drawables.end());
    drawables.insert(drawables.end(), std::make_move_iterator(approach_circles.begin()), std::make_move_iterator(approach_circles.end()));

    approach_circles.clear();

    if(replay.replay) {
        const auto current_frame = replay.frame_at(player.get_current_time());

        auto pos = Magnum::Vector2{current_frame.x, current_frame.y};
        pos = coordinate_provider.convert_point(pos, true);

        constexpr const auto ratio_cursor_cs = 5;
        const auto color = (current_frame.state > 0) ? Magnum::Color4::red() : Magnum::Color4::yellow();

        drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer,
                                                              pos,
                                                              osu::cs_to_osupixel(info_provider.cs()) / ratio_cursor_cs,
                                                              Circle_draw_options{
                                                                      .color = color,
                                                                      .circle_center = Circleobject_shader::filled}));
    }
}

Visible_hitobjects_manager::Visible_hitobjects_manager(const Beatmap_manager& beatmap, const Replay_manager& replay,
                                                       const Playfield_coordinate_provider& coordinate_provider, const Playback_logic& player)
    : beatmap{beatmap}, replay{replay}, coordinate_provider{coordinate_provider}, player{player}
{
}


void Visible_hitobjects_manager::draw(Magnum::GL::Framebuffer& framebuffer)
{
    for(auto& drawable : drawables) drawable->draw(framebuffer);
}

void Visible_hitobjects_manager::add_approach_circle(Magnum::Vector2 pos, std::chrono::milliseconds time, const Beatmap_info_provider& info_provider)
{
    const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(info_provider.ar()))};
    const auto early_duration = time - player.get_current_time();
    const auto progress = static_cast<float>(early_duration.count()) / static_cast<float>(early_window.count());
    const auto radius = osu::cs_to_osupixel(info_provider.cs()) * (1.f + 2 * progress);
    approach_circles.push_back(std::make_unique<Drawable_circle>(circle_renderer, pos, radius, Circle_draw_options{.circle_center = Circleobject_shader::hollow}));
}

void Visible_hitobjects_manager::draw_circle(Circle_object& circle, const Beatmap_info_provider& info_provider)
{
    const auto position = coordinate_provider.convert_point(circle.position);

    drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, position, osu::cs_to_osupixel(info_provider.cs()), Circle_draw_options{}));
    if(circle.time > player.get_current_time()) add_approach_circle(position, circle.time, info_provider);
}

void Visible_hitobjects_manager::draw_slider(Slider_object& slider, const Beatmap_info_provider& info_provider)
{
    if(slider.time > player.get_current_time()) add_approach_circle(coordinate_provider.convert_point(vector_o2m(slider.slider.points.front())), slider.time, info_provider);
    drawables.push_back(std::make_unique<Drawable_slider>(slider_renderer, circle_renderer, slider, coordinate_provider,
                                                          osu::cs_to_osupixel(info_provider.cs()), player.get_current_time()));
}
