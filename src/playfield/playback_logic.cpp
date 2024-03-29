#include "playback_logic.h"
#include <imgui.h>

void Playback_logic::update(std::chrono::milliseconds time_passed)
{
    if(paused) time_passed = std::chrono::milliseconds::zero();
    if(last_time == current_time && time_passed == std::chrono::milliseconds::zero()) return;

    // Hitobject stuff
    current_time += duration_cast<std::chrono::milliseconds>(speed * time_passed);//Todo: Handle fractions better
    if(current_time.count() > beatmap.time_range().y().count()) current_time = beatmap.time_range().x();

    last_time = current_time;
}
std::chrono::milliseconds Playback_logic::get_current_time() const
{
    return current_time;
}
void Playback_logic::draw()
{
    if(ImGui::Begin("Controls")) {
        const auto range = beatmap.time_range();

        auto time = static_cast<int>(get_current_time().count());
        ImGui::SliderInt("Time", &time, static_cast<int>(range.x().count()), static_cast<int>(range.y().count()));
        current_time = std::chrono::milliseconds{time};
        ImGui::InputFloat("Speed", &speed);
        ImGui::Checkbox("Paused", &paused);
    }
    ImGui::End();
}
Playback_logic::Playback_logic(const Analysed_beatmap& beatmap) : beatmap{beatmap}
{
}
