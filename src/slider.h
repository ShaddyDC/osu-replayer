#pragma once
#include <vector>
#include <string_view>
#include <optional>
#include <Magnum/Math/Vector2.h>

using Slider_point = Magnum::Math::Vector2<float>;

using Slider_segment = std::vector<Slider_point>;

using Slider = std::vector<Slider_segment>;

std::optional<Slider> parse_slider(const std::string_view slider_string);
