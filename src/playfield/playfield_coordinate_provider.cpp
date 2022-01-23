#include "playfield_coordinate_provider.h"

void Playfield_coordinate_provider::update(std::chrono::milliseconds /*passed_time*/)
{
    invert_y = replay.has_value() && osu::has_mods(replay->mods, osu::Mods::HardRock);
}

Playfield_coordinate_provider::Playfield_coordinate_provider(Magnum::Vector2 offset, const std::optional<osu::Replay>& replay)
    : offset{offset}, replay(replay), invert_y{false} {}

Magnum::Vector2 Playfield_coordinate_provider::convert_point(Magnum::Vector2 point) const
{
    constexpr const auto playfield_height = 384.f;
    if(invert_y) point.y() = playfield_height - point.y();
    return point + offset;
}
