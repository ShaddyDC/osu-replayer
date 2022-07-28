#ifndef SLIDER_DRAW_ANALYSED_BEATMAP_H
#define SLIDER_DRAW_ANALYSED_BEATMAP_H

#include "beatmap_info_provider.h"
#include "hitobjects.h"
#include "osu_reader/beatmap.h"
#include "util/bindable.h"

class Analysed_beatmap {
public:
    explicit Analysed_beatmap(Bindable<std::optional<osu::Beatmap>>& beatmap);

    [[nodiscard]] std::vector<Circle_object> circles_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider) const;
    [[nodiscard]] std::vector<Slider_object> sliders_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider) const;
    [[nodiscard]] Magnum::Math::Vector2<std::chrono::milliseconds> time_range() const;
    [[nodiscard]] const std::optional<osu::Beatmap>& get_beatmap() const;

    // Delete moves since callback relies on position
    Analysed_beatmap(Analysed_beatmap&&) = delete;
    Analysed_beatmap& operator=(Analysed_beatmap&&) = delete;
    Analysed_beatmap(const Analysed_beatmap&) = delete;
    Analysed_beatmap& operator=(const Analysed_beatmap&) = delete;
    ~Analysed_beatmap() = default;

private:
    void analyse(const Bindable<std::optional<osu::Beatmap>>& map);

    const std::optional<osu::Beatmap>& beatmap;
    std::vector<Circle_object> circles;
    std::vector<Slider_object> sliders;

    Magnum::Math::Vector2<std::chrono::milliseconds> calculated_time_range;
};


#endif//SLIDER_DRAW_ANALYSED_BEATMAP_H
