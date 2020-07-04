#pragma once
#include "hitobjects.h"
#include <string>

class Data_reader{
public:
    Data_reader();

    std::vector<Circle_object> circles_at(int time);
    std::vector<Slider_object> sliders_at(int time);
    Magnum::Math::Vector2<int> time_range();

    void map_window();
    void load_map(const int id);

    int current_id = 0;
    std::string map = "Not loaded";

private:
    std::vector<Circle_object> circles;
    std::vector<Slider_object> sliders;
};
