#ifndef SLIDER_DRAW_REPLAY_LOADER_H
#define SLIDER_DRAW_REPLAY_LOADER_H

#include "api_manager.h"
#include "bindable.h"
#include <optional>
#include <osu_reader/replay.h>

class Replay_loader {
public:
    explicit Replay_loader(Api_manager& api_manager);
    Bindable<std::optional<osu::Replay>> replay;

    void replay_window();
    [[nodiscard]] osu::Replay::Replay_frame frame_at(std::chrono::milliseconds time) const;

private:
    Api_manager& api_manager;
};

#endif// SLIDER_DRAW_REPLAY_LOADER_H
