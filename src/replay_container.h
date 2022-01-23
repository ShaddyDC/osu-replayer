#ifndef SLIDER_DRAW_REPLAY_CONTAINER_H
#define SLIDER_DRAW_REPLAY_CONTAINER_H

#include "api_manager.h"
#include <optional>
#include <osu_reader/replay.h>

class Replay_manager {
public:
    explicit Replay_manager(Api_manager& api_manager);
    std::optional<osu::Replay> replay;

    void replay_window();
    osu::Replay::Replay_frame frame_at(std::chrono::milliseconds time) const;

private:
    Api_manager& api_manager;
};

#endif// SLIDER_DRAW_REPLAY_CONTAINER_H
