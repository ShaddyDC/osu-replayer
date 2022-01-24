#include "playfield_size_manager.h"
#include <imgui.h>

void Playfield_size_manager::update(std::chrono::milliseconds  /*time_passed*/)
{
    // Set playfield size
    if(ImGui::Begin("Playfield")) {
        const auto window_size = ImGui::GetWindowSize();
        constexpr auto bottom_offset = 20.f;// Prevent scrollbar from appearing
        const float scale = std::min((window_size.y - bottom_offset) / static_cast<float>(size_unscaled.y()),
                                     window_size.x / static_cast<float>(size_unscaled.x()));

        scaling_size = static_cast<Magnum::Vector2i>(scale * size_unscaled);
    }
    ImGui::End();
}
Magnum::Vector2i Playfield_size_manager::get_size() const
{
    return scaling_size;
}
Magnum::Vector2i Playfield_size_manager::get_top_left() const
{
    return top_left;
}
Magnum::Vector2i Playfield_size_manager::get_bottom_right() const
{
    return bottom_right;
}
Magnum::Vector2i Playfield_size_manager::get_internal_size() const
{
    return size_unscaled;
}
