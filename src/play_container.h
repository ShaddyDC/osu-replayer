#pragma once

#include "render/circleobject_renderer.h"
#include "render/slider_renderer.h"
#include "render/line_renderer.h"
#include "data_reader.h"

#include <Magnum/GL/Texture.h>

struct Slider_meshtime {
    Slider_meshtime(Slider_mesh&& mesh, const int time): mesh{ std::move(mesh) }, time{ time } {};
    Slider_mesh mesh;
    int time;
};

struct Circle_meshtime {
    Circle_meshtime(Circleobject_mesh&& mesh, const int time): mesh{ std::move(mesh) }, time{ time } {};
    Circleobject_mesh mesh;
    int time;
};

class Play_container{
public:
    Play_container();
    void update(int time_passed);
    Magnum::GL::Texture2D draw();

    Data_reader data;

    std::vector<Slider_meshtime> sliders;
    std::vector<Circle_meshtime> circles;

    int current_time = 0;
    int last_time = current_time;
    bool paused = false;
    float speed = 1.f;

    const Magnum::Vector2i field_size = { 512, 384 };
    const Magnum::Vector2i top_left{ 50, 50 };
    const Magnum::Vector2i bottom_right{ top_left + field_size};
    const Magnum::Vector2i size = { bottom_right + top_left };
    float size_scale = 1.f;
    Magnum::Vector2i scaling_size = static_cast<Magnum::Vector2i>(size_scale * size);

    Line_mesh border_mesh;

private:
    Magnum::Vector2 to_screen(const Magnum::Vector2 point);

    Circleobject_renderer circle_renderer;
    Slider_renderer slider_renderer;
    Line_renderer line_renderer;
};