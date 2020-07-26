#pragma once
#include "hitobjects.h"
#include <string>
#include <osu_reader/beatmap.h>

class Data_reader{
public:
    Data_reader();

    std::vector<Circle_object> circles_at(std::chrono::milliseconds time);
    std::vector<Slider_object> sliders_at(std::chrono::milliseconds time);
    Magnum::Math::Vector2<std::chrono::milliseconds> time_range();

    void map_window();
    void load_map(const int id);

    int current_id = 0;
    std::string map_string = "Not loaded";
    std::optional<osu::Beatmap> map;

private:
    void init_map();

    std::vector<Circle_object> circles;
    std::vector<Slider_object> sliders;
};
