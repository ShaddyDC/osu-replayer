#pragma once

#include <osu_reader/beatmap.h>
#include <osu_reader/replay.h>

class Beatmap_info_provider {
public:
    explicit Beatmap_info_provider(const osu::Beatmap& beatmap, osu::Mods mods = {}) : beatmap{beatmap}, mods{mods} {}

    [[nodiscard]] inline float cs() const
    {
        if(osu::has_mods(mods, osu::Mods::HardRock)) return std::min(beatmap.cs * 1.3f, 10.f);
        if(osu::has_mods(mods, osu::Mods::Easy)) return 0.5f * beatmap.cs;
        return beatmap.cs;
    }

    [[nodiscard]] inline float ar() const
    {
        if(osu::has_mods(mods, osu::Mods::HardRock)) return std::min(beatmap.ar * 1.4f, 10.f);
        if(osu::has_mods(mods, osu::Mods::Easy)) return 0.5f * beatmap.ar;
        return beatmap.ar;
    }

    [[nodiscard]] inline float od() const
    {
        if(osu::has_mods(mods, osu::Mods::HardRock)) return std::min(beatmap.od * 1.4f, 10.f);
        if(osu::has_mods(mods, osu::Mods::Easy)) return 0.5f * beatmap.od;
        return beatmap.od;
    }

    [[nodiscard]] inline float hp() const
    {
        if(osu::has_mods(mods, osu::Mods::HardRock)) return std::min(beatmap.hp * 1.4f, 10.f);
        if(osu::has_mods(mods, osu::Mods::Easy)) return 0.5f * beatmap.hp;
        return beatmap.hp;
    }

private:

    const osu::Beatmap& beatmap;
    const osu::Mods mods;
};
