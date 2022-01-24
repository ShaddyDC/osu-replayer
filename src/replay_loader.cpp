#include "replay_loader.h"
#include <Corrade/Utility/Resource.h>
#include <imgui.h>
#include <osu_reader/replay_reader.h>

Replay_loader::Replay_loader(Api_manager& api_manager)
    : api_manager{api_manager}
{
    const Corrade::Utility::Resource rs{"data"};

    const auto replay_data = rs.get("example.osr");

    auto reader = osu::Replay_reader{};
    reader.parse_frames = true;

    replay = reader.from_string(replay_data);
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
        if(replay) {
            // Handle non-integer types below 64-bit. Fine since this is readonly anyway.
            const auto label_ints = [](const auto label, auto value) {
                auto v = static_cast<int>(value);
                ImGui::InputInt(label, &v, ImGuiInputTextFlags_ReadOnly);
            };

            ImGui::BeginDisabled();
            label_ints("Game Version", replay->game_version);
            ImGui::LabelText("Player", "%s", replay->player_name.c_str());
            ImGui::LabelText("Map hash", "%s", replay->map_hash.c_str());
            ImGui::LabelText("Replay hash", "%s", replay->replay_hash.c_str());
            label_ints("Count 300", replay->count_300);
            label_ints("Count 100", replay->count_100);
            label_ints("Count 50", replay->count_50);
            label_ints("Count Geki", replay->count_geki);
            label_ints("Count Katsu", replay->count_katsu);
            label_ints("Count Miss", replay->count_miss);
            label_ints("Score", replay->score);
            label_ints("Max combo", replay->max_combo);
            ImGui::Checkbox("FC", &replay->full_combo);
            label_ints("Mods", replay->mods);
            ImGui::InputScalar("Score ID", ImGuiDataType_U64, &replay->score_id);
            ImGui::EndDisabled();
        } else {
            ImGui::Text("Failed loading Replay");
        }
    }
    ImGui::End();
}
osu::Replay::Replay_frame Replay_loader::frame_at(std::chrono::milliseconds time) const
{
    if(!replay || !replay->frames || replay->frames->empty())
        return osu::Replay::Replay_frame{};

    return *std::min_element(replay->frames->begin(), replay->frames->end(),
                             [time](const auto& a, const auto& b) {
                                 return std::abs((a.time - time).count()) < std::abs((b.time - time).count());
                             });
}
