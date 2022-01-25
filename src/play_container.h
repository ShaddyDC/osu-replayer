#pragma once

#include "beatmap_reader.h"
#include "component.h"
#include "playfield/border.h"
#include "playfield/playback_logic.h"
#include "playfield/playfield_coordinate_provider.h"
#include "playfield/playfield_size_manager.h"
#include "playfield/visible_hitobjects_manager.h"
#include "replay_beatmap_matcher.h"

#include <Magnum/GL/Texture.h>

class Play_container : public Component {
public:
    explicit Play_container(Api_manager& api_manager);
    void update(std::chrono::milliseconds time_passed) override;
    void draw() override;
    [[nodiscard]] const Playfield_size_manager& get_size_manager() const;
    void set_score(std::uint64_t id);

    // Disable moves
    Play_container(const Play_container&) = delete;
    Play_container& operator=(const Play_container&) = delete;
    Play_container(Play_container&&) = delete;
    Play_container& operator=(Play_container&&) = delete;

    ~Play_container() override = default;

private:
    Magnum::GL::Texture2D generate_playfield_texture();
    Magnum::Vector2 to_screen(Magnum::Vector2 point);

    Beatmap_reader beatmap_container;
    Replay_loader replay_container;

    Analysed_beatmap beatmap;
    Analysed_replay replay;

    Replay_beatmap_matcher matcher;

    Playback_logic player;
    Playfield_size_manager size_manager;
    Playfield_coordinate_provider coordinate_provider;

    Visible_objects_manager objects_manager;

    Playfield_border border{size_manager.get_top_left(), size_manager.get_bottom_right()};

    Magnum::GL::Texture2D playfield;
};
