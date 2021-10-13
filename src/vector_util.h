#pragma once

#include <Magnum/Math/Vector2.h>
#include <osu_reader/vector2.h>

inline constexpr Magnum::Vector2 vector_o2m(osu::Vector2 v) { return {v.x, v.y}; }
inline osu::Vector2 vector_m2o(Magnum::Vector2 v) { return {v.x(), v.y()}; }
