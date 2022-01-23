#ifndef SLIDER_DRAW_PLAYBACK_LOGIC_H
#define SLIDER_DRAW_PLAYBACK_LOGIC_H

#include "data_reader.h"
#include "replay_container.h"
#include <chrono>

class Playback_logic {
public:
    explicit Playback_logic(const Beatmap_manager& data);
    void update(std::chrono::milliseconds time_passed);

    [[nodiscard]] std::chrono::milliseconds get_current_time() const;
    void draw();

private:
    std::chrono::milliseconds current_time = std::chrono::milliseconds::zero();
    std::chrono::milliseconds last_time = current_time;
    bool paused = false;
    float speed = 1.f;

    const Beatmap_manager& data;
};


#endif//SLIDER_DRAW_PLAYBACK_LOGIC_H
