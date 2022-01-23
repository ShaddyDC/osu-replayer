#ifndef SLIDER_DRAW_VISIBLE_HITOBJECTS_MANAGER_H
#define SLIDER_DRAW_VISIBLE_HITOBJECTS_MANAGER_H

#include "../render/drawable.h"
#include "data_reader.h"
#include "playback_logic.h"
#include "playfield_coordinate_provider.h"
#include "render/circleobject_renderer.h"
#include "render/slider_renderer.h"
#include "replay_container.h"
#include <optional>

class Visible_objects_manager {
public:
    Visible_objects_manager(const Beatmap_manager& beatmap, const Replay_manager& replay, const Playfield_coordinate_provider& coordinate_provider, const Playback_logic& player);

    void update(std::chrono::milliseconds time_passed);
    void draw(Magnum::GL::Framebuffer& framebuffer);

private:
    void add_approach_circle(Magnum::Vector2 pos, std::chrono::milliseconds time, const Beatmap_info_provider& info_provider);
    void add_circle(Circle_object& circle, const Beatmap_info_provider& info_provider);
    void add_slider(Slider_object& slider, const Beatmap_info_provider& info_provider);

    const Beatmap_manager& beatmap;
    const Replay_manager& replay;
    Drawables drawables;
    Drawables approach_circles;

    Circleobject_renderer circle_renderer;
    Slider_renderer slider_renderer;

    const Playfield_coordinate_provider& coordinate_provider;
    const Playback_logic& player;
};


#endif//SLIDER_DRAW_VISIBLE_HITOBJECTS_MANAGER_H
