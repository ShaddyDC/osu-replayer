#pragma once
#include <Magnum/Math/Vector2.h>
#include <optional>
#include <string_view>
#include <vector>

using Slider_point = Magnum::Math::Vector2<float>;

using Slider_segment = std::vector<Slider_point>;

using Slider = std::vector<Slider_segment>;

enum class Slider_type : char {
    bezier = 'B',
    catmull = 'C',
    perfect = 'P',
    linear = 'L'
};

std::optional<Slider> parse_slider(const std::string_view slider_string);
