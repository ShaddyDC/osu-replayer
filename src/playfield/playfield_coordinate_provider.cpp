#include "playfield_coordinate_provider.h"

constexpr const auto playfield_height = 384.f;

void Playfield_coordinate_provider::update(std::chrono::milliseconds /*passed_time*/)
{
    invert_y = replay.has_value() && osu::has_mods(replay->mods, osu::Mods::HardRock);
}

Playfield_coordinate_provider::Playfield_coordinate_provider(Magnum::Vector2 offset, const std::optional<osu::Replay>& replay)
    : offset{offset}, replay(replay), invert_y{false} {}

Magnum::Vector2 Playfield_coordinate_provider::osu_to_field(Magnum::Vector2 point, bool no_flip) const
{
    if(invert_y && !no_flip) point.y() = playfield_height - point.y();
    return point + offset;
}

Magnum::Vector2 Playfield_coordinate_provider::field_to_osu(Magnum::Vector2 point, bool no_flip) const
{
    point -= offset;
    if(invert_y && !no_flip) point.y() = playfield_height - point.y();
    return point;
}
