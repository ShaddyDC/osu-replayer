#pragma once
#include "api_manager.h"
#include "bindable.h"
#include <osu_reader/beatmap.h>
#include <string>

class Beatmap_reader : public Component {
public:
    explicit Beatmap_reader(Api_manager& api_manager);

    void draw() override;
    void load_map(int id);

    int current_id = 0;
    std::string map_string = "Not loaded";
    Bindable<std::optional<osu::Beatmap>> map;

private:
    void init_map();

    Api_manager& api_manager;
};
