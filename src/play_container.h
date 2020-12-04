#pragma once

#include "data_reader.h"
#include "render/circleobject_renderer.h"
#include "render/line_renderer.h"
#include "render/slider_renderer.h"
#include "replay_container.h"

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

class Play_container {
public:
    explicit Play_container(Api_manager& api_manager);
    void update(std::chrono::milliseconds time_passed);
    Magnum::GL::Texture2D draw();

    Data_reader data;
    Replay_container replay_container;

    std::vector<Slider_meshtime> sliders;
    std::vector<Circle_meshtime> circles;
    std::optional<Circleobject_mesh> current_cursor;

    std::chrono::milliseconds current_time = std::chrono::milliseconds::zero();
    std::chrono::milliseconds last_time = current_time;
    bool paused = false;
    float speed = 1.f;

    const Magnum::Vector2i field_size = {512, 384};
    const Magnum::Vector2i top_left{50, 50};
    const Magnum::Vector2i bottom_right{top_left + field_size};
    const Magnum::Vector2i size = {bottom_right + top_left};
    float size_scale = 1.f;
    Magnum::Vector2i scaling_size = static_cast<Magnum::Vector2i>(size_scale * size);

    Line_mesh border_mesh;

private:
    Magnum::Vector2 to_screen(const Magnum::Vector2 point);

    Circleobject_renderer circle_renderer;
    Slider_renderer slider_renderer;
    Line_renderer line_renderer;
};
