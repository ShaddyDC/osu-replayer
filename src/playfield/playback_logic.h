#ifndef SLIDER_DRAW_PLAYBACK_LOGIC_H
#define SLIDER_DRAW_PLAYBACK_LOGIC_H

#include <chrono>
#include "data_reader.h"
#include "replay_container.h"

class Playback_logic {
public:
    Playback_logic(const Data_reader& data);
    void update(std::chrono::milliseconds time_passed);

    std::chrono::milliseconds get_current_time();
    void draw();

private:
    std::chrono::milliseconds current_time = std::chrono::milliseconds::zero();
    std::chrono::milliseconds last_time = current_time;
    bool paused = false;
    float speed = 1.f;

    const Data_reader& data;
};


#endif//SLIDER_DRAW_PLAYBACK_LOGIC_H
