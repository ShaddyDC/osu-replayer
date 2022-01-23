#pragma once

#include "component.h"
#include "playfield/border.h"
#include "playfield/playback_logic.h"
#include "playfield/playfield_coordinate_provider.h"
#include "playfield/playfield_size_manager.h"
#include "render/circleobject_renderer.h"
#include "render/drawable.h"
#include "render/slider_renderer.h"

#include <Magnum/GL/Texture.h>

struct Slider_meshtime {
    Slider_meshtime(Slider_mesh&& mesh, const std::chrono::milliseconds time) : mesh{std::move(mesh)}, time{time} {};
    Slider_mesh mesh;
    std::chrono::milliseconds time;
};

struct Circle_meshtime {
    Circle_meshtime(Circleobject_mesh&& mesh, const std::chrono::milliseconds time) : mesh{std::move(mesh)}, time{time} {};
    Circleobject_mesh mesh;
    std::chrono::milliseconds time;
};

struct Approach_meshtime {
    Approach_meshtime(Circleobject_mesh&& mesh, const std::chrono::milliseconds time) : mesh{std::move(mesh)}, time{time} {};
    Circleobject_mesh mesh;
    std::chrono::milliseconds time;
};

class Play_container : public Component {
public:
    explicit Play_container(Api_manager& api_manager);
    void update(std::chrono::milliseconds time_passed) override;
    void draw() override;
    Magnum::Vector2i get_size();

private:
    Drawables drawables;

    Magnum::GL::Texture2D generate_playfield_texture();
    Magnum::Vector2 to_screen(Magnum::Vector2 point);

    Data_reader data;
    Replay_container replay_container;

    Playback_logic player;
    Playfield_size_manager size_manager;
    Playfield_coordinate_provider coordinate_provider;


    Playfield_border border{size_manager.get_top_left(), size_manager.get_bottom_right()};

    Magnum::GL::Texture2D playfield;
    Circleobject_renderer circle_renderer;
    Slider_renderer slider_renderer;
};
