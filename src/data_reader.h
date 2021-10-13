#pragma once
#include "api_manager.h"
#include "beatmap_info_provider.h"
#include "hitobjects.h"
#include <osu_reader/beatmap.h>
#include <string>

class Data_reader {
public:
    explicit Data_reader(Api_manager& api_manager);

    std::vector<Circle_object> circles_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider);
    std::vector<Slider_object> sliders_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider);
    Magnum::Math::Vector2<std::chrono::milliseconds> time_range();

    void map_window();
    void load_map(const int id);

    int current_id = 0;
    std::string map_string = "Not loaded";
    std::optional<osu::Beatmap> map;

private:
    void init_map();

    std::vector<Circle_object> circles;
    std::vector<Slider_object> sliders;

    Api_manager& api_manager;
};
