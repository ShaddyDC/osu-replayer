#ifndef SLIDER_DRAW_ANALYSED_REPLAY_H
#define SLIDER_DRAW_ANALYSED_REPLAY_H

#include "osu_reader/replay.h"
#include "util/bindable.h"
#include <optional>

class Analysed_replay {
public:
    explicit Analysed_replay(Bindable<std::optional<osu::Replay>>& replay);

    [[nodiscard]] const std::optional<osu::Replay>& get_replay() const;
    [[nodiscard]] osu::Replay::Replay_frame frame_at(std::chrono::milliseconds time) const;

    // Delete moves since callback relies on location
    Analysed_replay(Analysed_replay&&) = delete;
    Analysed_replay& operator=(Analysed_replay&&) = delete;
    Analysed_replay(const Analysed_replay&) = delete;
    Analysed_replay& operator=(const Analysed_replay&) = delete;
    ~Analysed_replay() = default;

private:
    const std::optional<osu::Replay>& replay;
};


#endif//SLIDER_DRAW_ANALYSED_REPLAY_H
