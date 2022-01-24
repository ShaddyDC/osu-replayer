#include "replay_loader.h"
#include <Corrade/Utility/Resource.h>
#include <imgui.h>
#include <osu_reader/replay_reader.h>

Replay_loader::Replay_loader(Api_manager& api_manager)
    : replay{std::nullopt}, api_manager{api_manager}
{
    const Corrade::Utility::Resource rs{"data"};

    const auto replay_data = rs.get("example.osr");

    auto reader = osu::Replay_reader{};
    reader.parse_frames = true;

    replay.set(reader.from_string(replay_data));
}
void Replay_loader::replay_window()
{
    //    if(ImGui::Begin("Load Replay")){
    //        ImGui::InputInt("id", &current_id);
    //        ImGui::SameLine();
    //        if(ImGui::Button("Load ID") && current_id > 0) load_map(current_id);
    //        ImGui::InputTextMultiline("Content", map_string.data(), map_string.size());
    //        if(ImGui::Button("Load String")) init_map();
    //    }
    //    ImGui::End();

    if(ImGui::Begin("Replay")) {
        if(replay.get()) {
            // Handle non-integer types below 64-bit. Fine since this is readonly anyway.
            const auto imgui_input = []<typename T>(const auto label, T value) {
                if constexpr(std::is_integral_v<T> && sizeof(T) >= sizeof(std::int64_t)) {
                    ImGui::InputScalar(label, ImGuiDataType_U64, &value);
                } else if constexpr(std::is_integral_v<T>) {
                    auto v = static_cast<int>(value);
                    ImGui::InputInt(label, &v, ImGuiInputTextFlags_ReadOnly);
                } else if constexpr(std::is_same_v<T, float>) {
                    ImGui::InputFloat(label, &value);
                } else if constexpr(std::is_same_v<T, bool>) {
                    ImGui::Checkbox(label, &value);
                }
            };
            ImGui::BeginDisabled();
            imgui_input("Game Version", replay.get()->game_version);
            ImGui::LabelText("Player", "%s", replay.get()->player_name.c_str());
            ImGui::LabelText("Map hash", "%s", replay.get()->map_hash.c_str());
            ImGui::LabelText("Replay hash", "%s", replay.get()->replay_hash.c_str());
            imgui_input("Count 300", replay.get()->count_300);
            imgui_input("Count 100", replay.get()->count_100);
            imgui_input("Count 50", replay.get()->count_50);
            imgui_input("Count Geki", replay.get()->count_geki);
            imgui_input("Count Katsu", replay.get()->count_katsu);
            imgui_input("Count Miss", replay.get()->count_miss);
            imgui_input("Score", replay.get()->score);
            imgui_input("Max combo", replay.get()->max_combo);
            imgui_input("FC", &replay.get()->full_combo);
            imgui_input("Mods", replay.get()->mods);
            imgui_input("Score ID", &replay.get()->score_id);
            ImGui::EndDisabled();
        } else {
            ImGui::Text("Failed loading Replay");
        }
    }
    ImGui::End();
}
