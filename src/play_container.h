#pragma once

#include "component.h"
#include "playfield/border.h"
#include "playfield/playback_logic.h"
#include "playfield/playfield_coordinate_provider.h"
#include "playfield/playfield_size_manager.h"
#include "playfield/visible_hitobjects_manager.h"

#include <Magnum/GL/Texture.h>

class Play_container : public Component {
public:
    explicit Play_container(Api_manager& api_manager);
    void update(std::chrono::milliseconds time_passed) override;
    void draw() override;
    Magnum::Vector2i get_size();

    // Disable moves
    Play_container(const Play_container&) = delete;
    Play_container& operator=(const Play_container&) = delete;
    Play_container(Play_container&&) = delete;
    Play_container& operator=(Play_container&&) = delete;

    ~Play_container() override = default;

private:
    Magnum::GL::Texture2D generate_playfield_texture();
    Magnum::Vector2 to_screen(Magnum::Vector2 point);

    Beatmap_manager beatmap_container;
    Replay_manager replay_container;

    Playback_logic player;
    Playfield_size_manager size_manager;
    Playfield_coordinate_provider coordinate_provider;

    Visible_objects_manager objects_manager;

    Playfield_border border{size_manager.get_top_left(), size_manager.get_bottom_right()};

    Magnum::GL::Texture2D playfield;
};
