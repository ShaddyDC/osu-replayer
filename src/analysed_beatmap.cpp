#include "analysed_beatmap.h"
#include "osu_reader/beatmap_util.h"
#include <Magnum/Magnum.h>
#include <algorithm>

Analysed_beatmap::Analysed_beatmap(Bindable<std::optional<osu::Beatmap>>& beatmap)
    : beatmap{beatmap.get()}
{
    beatmap.register_callback([this](const Bindable<std::optional<osu::Beatmap>>& beatmap) {
        analyse(beatmap);
    });
    analyse(beatmap);
}

void Analysed_beatmap::analyse(const Bindable<std::optional<osu::Beatmap>>& map)
{
    Corrade::Utility::Debug() << "Beatmap updated, analysing new one";
    if(map.get() && map.get()->mode == osu::Gamemode::osu) {
        circles.clear();
        sliders.clear();

        for(const auto& circle : map.get()->circles) {
            circles.emplace_back(Magnum::Vector2{circle.pos.x, circle.pos.y}, circle.time);
        }

        for(const auto& slider : map.get()->sliders) {
            sliders.emplace_back(slider, slider.time);
        }
    } else {
        Corrade::Utility::Debug() << "Not a valid osu mode beatmap";
    }
}

std::vector<Circle_object> Analysed_beatmap::circles_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider) const
{
    std::vector<Circle_object> ret;

    const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(info_provider.ar()))};
    const auto late_window = std::chrono::milliseconds{static_cast<int>(osu::od_to_ms300(info_provider.od()))};

    const auto in_time = [time, early_window, late_window](const auto& obj) {
        return (obj.time - early_window).count() < time.count() && (obj.time + late_window).count() > time.count();
    };

    std::copy_if(circles.cbegin(), circles.cend(), std::back_inserter(ret), in_time);
    return ret;
}

std::vector<Slider_object> Analysed_beatmap::sliders_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider) const
{
    std::vector<Slider_object> ret;

    const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(info_provider.ar()))};
    const auto late_window = std::chrono::milliseconds{static_cast<int>(osu::od_to_ms300(info_provider.od()))};

    const auto in_time = [time, early_window, late_window](const auto& obj) {
        return (obj.time - early_window).count() < time.count() && (obj.time + obj.slider.duration + late_window).count() > time.count();
    };

    std::copy_if(sliders.cbegin(), sliders.cend(), std::back_inserter(ret), in_time);
    return ret;
}

Magnum::Math::Vector2<std::chrono::milliseconds> Analysed_beatmap::time_range() const
{
    using namespace std::chrono_literals;
    const auto start = min(sliders.front().time, circles.front().time);//Todo: Bounds Check
    const auto end = max(sliders.back().time, circles.back().time);
    return {start - 5s, end + 5s};
}

const std::optional<osu::Beatmap>& Analysed_beatmap::get_beatmap() const
{
    return beatmap;
}
