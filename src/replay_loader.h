#ifndef SLIDER_DRAW_REPLAY_LOADER_H
#define SLIDER_DRAW_REPLAY_LOADER_H

#include "api_manager.h"
#include "bindable.h"
#include <optional>
#include <osu_reader/replay.h>

class Replay_loader {
public:
    explicit Replay_loader(Api_manager& api_manager);

    void load_replay(std::uint64_t score_id);
    void replay_window();

    Bindable<std::optional<osu::Replay>> replay;


private:
    Api_manager& api_manager;
    std::uint64_t id = 0;
};

#endif// SLIDER_DRAW_REPLAY_LOADER_H
