#include "replay_beatmap_matcher.h"
#include <Corrade/Utility/Debug.h>

Replay_beatmap_matcher::Replay_beatmap_matcher(Bindable<std::optional<osu::Replay>>& replay, Beatmap_reader& beatmap_reader, Api_manager& api)
    : beatmap_reader{beatmap_reader}, api{api}
{
    replay.register_callback([this](const Bindable<std::optional<osu::Replay>>& replay) {
        update(replay);
    });
}

void Replay_beatmap_matcher::update(const Bindable<std::optional<osu::Replay>>& replay)
{
    if(!replay.get()){
        Corrade::Utility::Debug() << "No replay to match map";
        return;
    }
    const auto map_id = api.beatmap_id(replay.get()->map_hash);
    if(!map_id) {
        Corrade::Utility::Debug() << "Couldn't get map id";
        return;
    }
    beatmap_reader.load_map(*map_id);
}
