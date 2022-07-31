#include "beatmap_reader.h"
#include "osu_reader/beatmap_parser.h"
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/Resource.h>
#include <imgui.h>


Beatmap_reader::Beatmap_reader(Api_manager& api_manager) : map{std::nullopt}, api_manager{api_manager}
{
    const Corrade::Utility::Resource rs{"data"};

    map_string = rs.getString("example.osu");

    init_map();
}

void Beatmap_reader::draw()
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
        if(map.get()) {
            // This is okay since this is all read-only
            const auto imgui_input = []<typename T>(const auto label, T value) {
                if constexpr(std::is_integral_v<T>) {
                    auto v = static_cast<int>(value);
                    ImGui::InputInt(label, &v);
                } else if constexpr(std::is_same_v<T, float>)
                    ImGui::InputFloat(label, &value);
            };
            ImGui::LabelText("Title", "%s", map.get()->title.c_str());
            ImGui::LabelText("Artist", "%s", map.get()->artist.c_str());
            ImGui::LabelText("Creator", "%s", map.get()->creator.c_str());
            ImGui::LabelText("Difficulty", "%s", map.get()->difficulty_name.c_str());
            ImGui::LabelText("Source", "%s", map.get()->source.c_str());
            imgui_input("Set ID", map.get()->beatmap_set_id);
            imgui_input("Map ID", map.get()->beatmap_id);
            ImGui::Text("[Difficulty]");
            imgui_input("HP", map.get()->hp);
            imgui_input("CS", map.get()->cs);
            imgui_input("OD", map.get()->od);
            imgui_input("AR", map.get()->ar);
            imgui_input("Slider Multiplier", &map.get()->slider_multiplier);
            imgui_input("Slider Tickrate", &map.get()->slider_tick_rate);
            ImGui::BeginDisabled();
            ImGui::Text("[Hitobjects]");
            imgui_input("Circles", map.get()->circles.size());
            imgui_input("Sliders", map.get()->sliders.size());
            imgui_input("Spinners", map.get()->spinners.size());
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

    if(map_string.empty()) {
        Corrade::Utility::Debug() << "Warning, empty map string";
    }

    init_map();
}

void Beatmap_reader::init_map()
{
    osu::Beatmap_parser parser{};
    parser.slider_paths = true;
    map.set(parser.from_string(map_string));
}
