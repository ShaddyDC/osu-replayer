#include "visible_hitobjects_manager.h"
#include "data/beatmap_info_provider.h"
#include "osu_reader/beatmap_util.h"
#include "render/drawable_circle.h"
#include "render/drawable_slider.h"
#include <algorithm>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

template<class Visitor>
static void iterate_hitobjects(const std::vector<Circle_object*>& circles, const std::vector<Slider_object*>& sliders, Visitor&& f)
{
    auto circles_it = circles.begin();
    auto sliders_it = sliders.begin();
    while(circles_it != circles.end() || sliders_it != sliders.end()) {
        if(circles_it == circles.end() || (sliders_it != sliders.end() && (*circles_it)->time >= (*sliders_it)->time)) {
            if(!f(*sliders_it)) return;
            ++sliders_it;
        } else {
            if(!f(*circles_it)) return;
            ++circles_it;
        }
    }
}

static void move_slider(Slider_object& slider, const osu::Vector2 pos)
{
    if(slider.slider.points.empty()) return;

    auto p = ranges::min_element(slider.slider.points, [&pos](const auto& a, const auto& b) { return length_squared(pos - a) < length_squared(pos - b); });
    *p = pos;

    slider.slider.distances.clear();
    slider.slider.distances.reserve(slider.slider.points.size());
    slider.slider.distances.push_back(0.f);

    for(auto it = slider.slider.points.cbegin() + 1; it != slider.slider.points.cend(); ++it)
        slider.slider.distances.push_back(distance(*it, *(it - 1)));
}

void Visible_objects_manager::update(std::chrono::milliseconds /*time_passed*/)
{
    if(!beatmap.get_beatmap()) return;

    drawables.clear();

    const auto mods = replay.get_replay() ? replay.get_replay()->mods : osu::Mods{};
    const Beatmap_info_provider info_provider{*beatmap.get_beatmap(), mods};

    auto circles = beatmap.circles_at(player.get_current_time(), info_provider);
    auto sliders = beatmap.sliders_at(player.get_current_time(), info_provider);

    // Check if hitobject was clicked
    click_handled &= mouse_provider.is_down();// Mouse down => unhandled
    if(mouse_provider.was_clicked() && !click_handled) {
        const auto point_in_range = [&](const osu::Vector2& p) { return distance(p, mouse_provider.get_coordinates()) < osu::cs_to_osupixel(info_provider.cs()); };
        iterate_hitobjects(circles, sliders, [&](const auto& object) {
            using T = std::decay_t<decltype(object)>;
            const auto already_selected = selection && std::holds_alternative<T>(*selection) && object == std::get<T>(*selection);

            if constexpr(std::is_same_v<T, Slider_object*>) {
                if(object->slider.points | ranges::views::filter(point_in_range)) {// TODO Handle large point distance
                    selection = object;                                            // TODO Invalidate selection value when beatmap changes
                    click_handled = !already_selected;                             // Allow interacting with selection
                    return false;
                }
            } else if constexpr(std::is_same_v<T, Circle_object*>) {
                if(point_in_range(vector_m2o(object->position))) {
                    selection = object;
                    click_handled = true;
                    return false;
                }
            } else {
                throw std::logic_error{"Invalid type"};
            }
            return true;
        });
    }

    // Create drawings in reverse order and reverse
    iterate_hitobjects(circles, sliders, [&](auto& object) {
        using T = std::decay_t<decltype(object)>;

        const auto is_selected = selection && std::holds_alternative<T>(*selection) && object == std::get<T>(*selection);

        if constexpr(std::is_same_v<T, Slider_object*>) {
            if(is_selected && mouse_provider.is_down() && !click_handled && modifiable_sliders) {
                move_slider(*object, mouse_provider.get_coordinates());
            }
            add_slider(*object, info_provider, is_selected);
        } else if constexpr(std::is_same_v<T, Circle_object*>) {
            add_circle(*object, info_provider, is_selected);
        } else {
            throw std::logic_error{"Invalid type"};
        }
        return true;
    });
    std::reverse(drawables.begin(), drawables.end());

    // Approach circles need to be in front of other hitobjects, so add them to the back
    drawables.insert(drawables.end(), std::make_move_iterator(approach_circles.begin()), std::make_move_iterator(approach_circles.end()));
    approach_circles.clear();

    // If replay available, draw cursor
    if(replay.get_replay()) {
        const auto current_frame = replay.frame_at(player.get_current_time());

        auto pos = Magnum::Vector2{current_frame.x, current_frame.y};
        pos = coordinate_provider.osu_to_field(pos, true);

        const auto color = (current_frame.state > 0) ? Magnum::Color4::red() : Magnum::Color4::yellow();
        constexpr const auto ratio_cursor_cs = 5;

        drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer,
                                                              pos,
                                                              osu::cs_to_osupixel(info_provider.cs()) / ratio_cursor_cs,
                                                              Circle_draw_options{
                                                                      .color = color,
                                                                      .circle_center = Circleobject_shader::filled}));
    }
}

Visible_objects_manager::Visible_objects_manager(Analysed_beatmap& beatmap, const Analysed_replay& replay,
                                                 const Playfield_coordinate_provider& coordinate_provider, const Playback_logic& player,
                                                 const Mouse_provider& mouse_provider, const bool& modifiable_sliders)
    : beatmap{beatmap}, replay{replay}, coordinate_provider{coordinate_provider}, player{player}, mouse_provider{mouse_provider},
      modifiable_sliders{modifiable_sliders}
{
}


void Visible_objects_manager::draw(Magnum::GL::Framebuffer& framebuffer)
{
    for(auto& drawable : drawables) drawable->draw(framebuffer);
}

void Visible_objects_manager::add_approach_circle(Magnum::Vector2 pos, std::chrono::milliseconds time, const Beatmap_info_provider& info_provider)
{
    const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(info_provider.ar()))};
    const auto early_duration = time - player.get_current_time();
    const auto progress = static_cast<float>(early_duration.count()) / static_cast<float>(early_window.count());
    const auto radius = osu::cs_to_osupixel(info_provider.cs()) * (1.f + 2 * progress);
    approach_circles.push_back(std::make_unique<Drawable_circle>(circle_renderer, pos, radius, Circle_draw_options{.circle_center = Circleobject_shader::hollow}));
}

void Visible_objects_manager::add_circle(const Circle_object& circle, const Beatmap_info_provider& info_provider, bool is_selected)
{
    const auto position = coordinate_provider.osu_to_field(circle.position);

    const auto color = is_selected ? Magnum::Color4::yellow() : Magnum::Color4{1};

    drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, position, osu::cs_to_osupixel(info_provider.cs()), Circle_draw_options{.color = color}));
    if(circle.time > player.get_current_time()) add_approach_circle(position, circle.time, info_provider);
}

void Visible_objects_manager::add_slider(const Slider_object& slider, const Beatmap_info_provider& info_provider, bool is_selected)
{
    if(slider.time > player.get_current_time()) add_approach_circle(coordinate_provider.osu_to_field(vector_o2m(slider.slider.points.front())), slider.time, info_provider);
    drawables.push_back(std::make_unique<Drawable_slider>(slider_renderer, circle_renderer, slider, is_selected, coordinate_provider,
                                                          osu::cs_to_osupixel(info_provider.cs()), player.get_current_time()));
}
