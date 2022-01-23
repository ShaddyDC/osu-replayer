#pragma once

#include <Magnum/Math/Vector2.h>

class Playfield_coordinate_provider {
public:
    Playfield_coordinate_provider(Magnum::Vector2 offset, bool invert_y) : offset{offset}, invert_y{invert_y} {}

    [[nodiscard]] inline Magnum::Vector2 convert_point(Magnum::Vector2 point) const
    {
        constexpr const auto playfield_height = 384.f;
        if(invert_y) point.y() = playfield_height - point.y();
        return point + offset;
    }

private:
    Magnum::Vector2 offset;
    bool invert_y;
};
