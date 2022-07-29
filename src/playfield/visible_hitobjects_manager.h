#ifndef SLIDER_DRAW_VISIBLE_HITOBJECTS_MANAGER_H
#define SLIDER_DRAW_VISIBLE_HITOBJECTS_MANAGER_H

#include "../render/drawable.h"
#include "data/analysed_beatmap.h"
#include "data/analysed_replay.h"
#include "mouse_provider.h"
#include "playback_logic.h"
#include "playfield_coordinate_provider.h"
#include "render/circleobject_renderer.h"
#include "render/slider_renderer.h"
#include <optional>
#include <variant>

class Visible_objects_manager {
public:
    Visible_objects_manager(Analysed_beatmap& beatmap, const Analysed_replay& replay, const Playfield_coordinate_provider& coordinate_provider, const Playback_logic& player,
                            const Mouse_provider& mouse_provider);

    void update(std::chrono::milliseconds time_passed);
    void draw(Magnum::GL::Framebuffer& framebuffer);

private:
    void add_approach_circle(Magnum::Vector2 pos, std::chrono::milliseconds time, const Beatmap_info_provider& info_provider);
    void add_circle(const Circle_object& circle, const Beatmap_info_provider& info_provider, bool is_selected);
    void add_slider(const Slider_object& slider, const Beatmap_info_provider& info_provider, bool is_selected);

    Analysed_beatmap& beatmap;
    const Analysed_replay& replay;
    Drawables drawables;
    Drawables approach_circles;

    Circleobject_renderer circle_renderer;
    Slider_renderer slider_renderer;

    const Playfield_coordinate_provider& coordinate_provider;
    const Playback_logic& player;
    const Mouse_provider& mouse_provider;

    bool click_handled = false;
    std::optional<std::variant<Circle_object*, Slider_object*>> selection;
};


#endif//SLIDER_DRAW_VISIBLE_HITOBJECTS_MANAGER_H
