#include "analysed_beatmap.h"
#include "osu_reader/beatmap_util.h"
#include "range/v3/algorithm.hpp"
#include "range/v3/range.hpp"
#include "range/v3/view.hpp"
#include <Magnum/Magnum.h>

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

    std::optional<std::chrono::milliseconds> start = {};
    std::optional<std::chrono::milliseconds> end = {};
    if(!circles.empty()) {
        const auto [min, max] = ranges::minmax_element(circles, std::less{}, &Circle_object::time);
        if(!start || *start > min->time) start = min->time;
        if(!end || *end < min->time) end = min->time;
    }
    if(!sliders.empty()) {
        const auto min = ranges::min_element(sliders, std::less{}, &Slider_object::time);
        const auto max = ranges::max_element(sliders, [](const auto& a, const auto& b) { return a.slider.time + a.slider.duration < b.slider.time + b.slider.duration; });
        if(!start || *start > min->time) start = min->time;
        if(!end || *end < max->slider.time + max->slider.duration) end = max->slider.time + max->slider.duration;
    }
    using namespace std::chrono_literals;
    calculated_time_range = {start.value_or(0s) - 5s, end.value_or(0s) + 5s};
}

std::vector<Circle_object*> Analysed_beatmap::circles_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider)
{
    const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(info_provider.ar()))};
    const auto late_window = std::chrono::milliseconds{static_cast<int>(osu::od_to_ms300(info_provider.od()))};

    const auto in_time = [time, early_window, late_window](const auto& obj) {
        return (obj.time - early_window).count() < time.count() && (obj.time + late_window).count() > time.count();
    };

    return circles | ranges::views::filter(in_time) | ranges::views::transform([](auto& o) { return &o; }) | ranges::to_vector;
}

std::vector<Slider_object*> Analysed_beatmap::sliders_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider)
{
    const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(info_provider.ar()))};
    const auto late_window = std::chrono::milliseconds{static_cast<int>(osu::od_to_ms300(info_provider.od()))};

    const auto in_time = [time, early_window, late_window](const auto& obj) {
        return (obj.time - early_window).count() < time.count() && (obj.time + obj.slider.duration + late_window).count() > time.count();
    };

    return sliders | ranges::views::filter(in_time) | ranges::views::transform([](auto& o) { return &o; }) | ranges::to_vector;
}

Magnum::Math::Vector2<std::chrono::milliseconds> Analysed_beatmap::time_range() const
{
    return calculated_time_range;
}

const std::optional<osu::Beatmap>& Analysed_beatmap::get_beatmap() const
{
    return beatmap;
}
