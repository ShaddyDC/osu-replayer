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