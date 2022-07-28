#include "analysed_replay.h"

Analysed_replay::Analysed_replay(Bindable<std::optional<osu::Replay>>& replay)
: replay{replay.get()}
{
}

const std::optional<osu::Replay>& Analysed_replay::get_replay()const
{
    return replay;
}

osu::Replay::Replay_frame Analysed_replay::frame_at(std::chrono::milliseconds time) const
{
    if(!replay || !replay->frames || replay->frames->empty())
        return osu::Replay::Replay_frame{};

    return *std::min_element(replay->frames->cbegin(), replay->frames->cend(),
                             [time](const auto& a, const auto& b) {
                                 return std::abs((a.time - time).count()) < std::abs((b.time - time).count());
                             });
}
