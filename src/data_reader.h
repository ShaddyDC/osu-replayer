#pragma once
#include "slider.h"
#include <Magnum/Math/Vector2.h>

struct Slider_object{
    Slider_object(const Slider& slider, const int time): slider{ slider }, time{ time } {};
    Slider slider;
    int time;
};

struct Circle_object{
    Circle_object(Magnum::Math::Vector2<float> position, const int time): position{ position }, time{ time } {};
    Magnum::Math::Vector2<float> position;
    int time;
};

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
