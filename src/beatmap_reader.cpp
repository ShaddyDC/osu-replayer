#include "beatmap_reader.h"
#include <Corrade/Utility/Resource.h>
#include <Magnum/Magnum.h>
#include <algorithm>
#include <imgui.h>
#include <osu_reader/beatmap_parser.h>
#include <osu_reader/beatmap_util.h>

using namespace std::chrono_literals;

Beatmap_reader::Beatmap_reader(Api_manager& api_manager) : api_manager{api_manager}
{
    const Corrade::Utility::Resource rs{"data"};

    map_string = rs.get("example.osu");

    init_map();
}

std::vector<Circle_object> Beatmap_reader::circles_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider) const
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

std::vector<Slider_object> Beatmap_reader::sliders_at(std::chrono::milliseconds time, const Beatmap_info_provider& info_provider) const
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


Magnum::Math::Vector2<std::chrono::milliseconds> Beatmap_reader::time_range() const
{
    const auto start = min(sliders.front().time, circles.front().time);//Todo: Bounds Check
    const auto end = max(sliders.back().time, circles.back().time);
    return {start - 5s, end + 5s};
}

void Beatmap_reader::map_window()
{
    if(ImGui::Begin("Load Map")) {
        ImGui::InputInt("id", &current_id);
        ImGui::SameLine();
        if(ImGui::Button("Load ID") && current_id > 0) load_map(current_id);
        ImGui::InputTextMultiline("Content", map_string.data(), map_string.size());
        if(ImGui::Button("Load String")) init_map();
    }
    ImGui::End();

    if(ImGui::Begin("Beatmap")) {
        if(map) {
            // This is okay since this is all read-only
            const auto input_int = [](const auto label, auto value) {
                int v = value;
                ImGui::InputInt(label, &v);
            };
            ImGui::LabelText("Title", "%s", map->title.c_str());
            ImGui::LabelText("Artist", "%s", map->artist.c_str());
            ImGui::LabelText("Creator", "%s", map->creator.c_str());
            ImGui::LabelText("Difficulty", "%s", map->difficulty_name.c_str());
            ImGui::LabelText("Source", "%s", map->source.c_str());
            ImGui::InputInt("Set ID", &map->beatmap_set_id);
            ImGui::InputInt("Map ID", &map->beatmap_id);
            ImGui::Text("[Difficulty]");
            ImGui::InputFloat("HP", &map->hp);
            ImGui::InputFloat("CS", &map->cs);
            ImGui::InputFloat("OD", &map->od);
            ImGui::InputFloat("AR", &map->ar);
            ImGui::InputFloat("Slider Multiplier", &map->slider_multiplier);
            ImGui::InputFloat("Slider Tickrate", &map->slider_tick_rate);
            ImGui::Text("[Hitobjects]");
            ImGui::BeginDisabled();
            input_int("Circles", circles.size());
            input_int("Sliders", sliders.size());
            input_int("Spinners", -1);
            ImGui::EndDisabled();
        } else {
            ImGui::Text("Failed loading map");
        }
    }
    ImGui::End();
}

void Beatmap_reader::load_map(const int id)
{
    auto bm = api_manager.beatmap(std::to_string(id));
    if(!bm) {
        Corrade::Utility::Debug() << "Couldn't load map";
        return;
    }
    map_string = std::move(*bm);

    if(map_string.empty()){
        Corrade::Utility::Debug() << "Warning, empty map string";
    }

    init_map();
}

void Beatmap_reader::init_map()
{
    osu::Beatmap_parser parser{};
    parser.slider_paths = true;
    map = parser.from_string(map_string);

    if(map && map->mode == osu::Gamemode::osu) {
        circles.clear();
        sliders.clear();

        for(const auto& circle : map->circles) {
            circles.emplace_back(Magnum::Vector2{circle.pos.x, circle.pos.y}, circle.time);
        }

        for(const auto& slider : map->sliders) {
            sliders.emplace_back(slider, slider.time);
        }
    }
}
