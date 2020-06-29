#pragma once
#include "hitobjects.h"

class Data_reader{
public:
    Data_reader();

    std::vector<Circle_object> circles_at(int time);
    std::vector<Slider_object> sliders_at(int time);
    Magnum::Math::Vector2<int> time_range();

private:
    std::vector<Circle_object> circles;
    std::vector<Slider_object> sliders;
};
