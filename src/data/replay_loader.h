#ifndef SLIDER_DRAW_REPLAY_LOADER_H
#define SLIDER_DRAW_REPLAY_LOADER_H

#include "features/api_manager.h"
#include "util/bindable.h"
#include <optional>
#include <osu_reader/replay.h>

class Replay_loader : public Component {
public:
    explicit Replay_loader(Api_manager& api_manager);

    void draw() override;
    void load_replay(std::uint64_t score_id);

    Bindable<std::optional<osu::Replay>> replay;

private:
    Api_manager& api_manager;
    std::uint64_t id = 0;
};

#endif// SLIDER_DRAW_REPLAY_LOADER_H
