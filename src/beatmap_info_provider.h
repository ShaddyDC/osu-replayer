#pragma once

#include <osu_reader/beatmap.h>
#include <osu_reader/replay.h>

class Beatmap_info_provider {
public:
    Beatmap_info_provider(const osu::Beatmap& beatmap, osu::Mods mods = {}) : beatmap{beatmap}, mods{mods} {}

    inline float cs() const
    {
        if(is_hr(mods)) return std::min(beatmap.cs * 1.3f, 10.f);
        if(is_ez(mods)) return 0.5f * beatmap.cs;
        return beatmap.cs;
    }

    inline float ar() const
    {
        if(is_hr(mods)) return std::min(beatmap.ar * 1.4f, 10.f);
        if(is_ez(mods)) return 0.5f * beatmap.ar;
        return beatmap.ar;
    }

    inline float od() const
    {
        if(is_hr(mods)) return std::min(beatmap.od * 1.4f, 10.f);
        if(is_ez(mods)) return 0.5f * beatmap.od;
        return beatmap.od;
    }

    inline float hp() const
    {
        if(is_hr(mods)) return std::min(beatmap.hp * 1.4f, 10.f);
        if(is_ez(mods)) return 0.5f * beatmap.hp;
        return beatmap.hp;
    }

private:
    inline bool is_hr(osu::Mods mods) const
    {
        return (mods & 16) > 0;
    }
    inline bool is_ez(osu::Mods mods) const
    {
        return (mods & 2) > 0;
    }

    const osu::Beatmap& beatmap;
    const osu::Mods mods;
};
