#pragma once

#include "osu_reader/replay.h"
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector2.h>
#include <chrono>
#include <optional>

class Playfield_coordinate_provider {
public:
    Playfield_coordinate_provider(Magnum::Vector2 offset, const std::optional<osu::Replay>& replay);

    void update(std::chrono::milliseconds time_passed);

    [[nodiscard]] Magnum::Vector2 convert_point(Magnum::Vector2 point, bool no_flip = false) const;

private:
    Magnum::Vector2 offset;
    const std::optional<osu::Replay>& replay;
    bool invert_y;
};
