#ifndef SLIDER_DRAW_PLAYBACK_LOGIC_H
#define SLIDER_DRAW_PLAYBACK_LOGIC_H

#include "data/analysed_beatmap.h"
#include "data/replay_loader.h"
#include <chrono>

class Playback_logic {
public:
    explicit Playback_logic(const Analysed_beatmap& beatmap);
    void update(std::chrono::milliseconds time_passed);

    [[nodiscard]] std::chrono::milliseconds get_current_time() const;
    void draw();

private:
    std::chrono::milliseconds current_time = std::chrono::milliseconds::zero();
    std::chrono::milliseconds last_time = current_time;
    bool paused = false;
    float speed = 1.f;

    const Analysed_beatmap& beatmap;
};


#endif//SLIDER_DRAW_PLAYBACK_LOGIC_H
