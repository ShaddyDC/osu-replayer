#pragma once

#include "beatmap_reader.h"
#include "playfield/border.h"
#include "playfield/mouse_provider.h"
#include "playfield/playback_logic.h"
#include "playfield/playfield_coordinate_provider.h"
#include "playfield/playfield_size_manager.h"
#include "playfield/visible_hitobjects_manager.h"
#include "replay_beatmap_matcher.h"
#include "util/component.h"

#include <Magnum/GL/Texture.h>

class Play_container : public Component {
public:
    explicit Play_container(Bindable<std::optional<osu::Beatmap>>& beatmap, Bindable<std::optional<osu::Replay>>& replay, const bool& modifiable_sliders);
    void update(std::chrono::milliseconds time_passed) override;
    void draw() override;
    [[nodiscard]] const Playfield_size_manager& get_size_manager() const;

    // Disable moves
    Play_container(const Play_container&) = delete;
    Play_container& operator=(const Play_container&) = delete;
    Play_container(Play_container&&) = delete;
    Play_container& operator=(Play_container&&) = delete;

    ~Play_container() override = default;

private:
    Magnum::GL::Texture2D generate_playfield_texture();
    void draw_playfield();

    Analysed_beatmap beatmap;
    Analysed_replay replay;

    Playback_logic player;
    Playfield_size_manager size_manager;
    Playfield_coordinate_provider coordinate_provider;
    Mouse_provider mouse_provider;

    Visible_objects_manager objects_manager;

    Playfield_border border{size_manager.get_top_left(), size_manager.get_bottom_right()};

    Magnum::GL::Texture2D playfield;
};
