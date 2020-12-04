#pragma once

#include "slider.h"
#include <Magnum/Math/Vector2.h>
#include <chrono>
#include <utility>

struct Slider_object {
    Slider_object(Slider  slider, const std::chrono::milliseconds time) : slider{std::move(slider)}, time{time} {};
    Slider slider;
    std::chrono::milliseconds time;
};

struct Circle_object {
    Circle_object(Magnum::Math::Vector2<float> position, const std::chrono::milliseconds time) : position{position}, time{time} {};
    Magnum::Math::Vector2<float> position;
    std::chrono::milliseconds time;
};