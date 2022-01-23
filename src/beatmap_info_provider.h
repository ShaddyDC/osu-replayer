#pragma once

#include <osu_reader/beatmap.h>
#include <osu_reader/replay.h>

class Beatmap_info_provider {
public:
    explicit Beatmap_info_provider(const osu::Beatmap& beatmap, osu::Mods mods = {}) : beatmap{beatmap}, mods{mods} {}

    [[nodiscard]] inline float cs() const
    {
        if(osu::has_mods(mods, osu::Mods::HardRock)) return std::min(beatmap.cs * hr_cs_mod, hr_max);
        if(osu::has_mods(mods, osu::Mods::Easy)) return ez_mod * beatmap.cs;
        return beatmap.cs;
    }

    [[nodiscard]] inline float ar() const
    {
        if(osu::has_mods(mods, osu::Mods::HardRock)) return std::min(beatmap.ar * hr_general_mod, hr_max);
        if(osu::has_mods(mods, osu::Mods::Easy)) return ez_mod * beatmap.ar;
        return beatmap.ar;
    }

    [[nodiscard]] inline float od() const
    {
        if(osu::has_mods(mods, osu::Mods::HardRock)) return std::min(beatmap.od * hr_general_mod, hr_max);
        if(osu::has_mods(mods, osu::Mods::Easy)) return ez_mod * beatmap.od;
        return beatmap.od;
    }

    [[nodiscard]] inline float hp() const
    {
        if(osu::has_mods(mods, osu::Mods::HardRock)) return std::min(beatmap.hp * hr_general_mod, hr_max);
        if(osu::has_mods(mods, osu::Mods::Easy)) return ez_mod * beatmap.hp;
        return beatmap.hp;
    }

private:
    constexpr static inline const auto hr_general_mod = 1.4f;
    constexpr static inline const auto hr_cs_mod = 1.3f;
    constexpr static inline const auto hr_max = 10.f;
    constexpr static inline const auto ez_mod = 0.5f;

    const osu::Beatmap& beatmap;
    const osu::Mods mods;
};
