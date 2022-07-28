#ifndef SLIDER_DRAW_REPLAY_BEATMAP_MATCHER_H
#define SLIDER_DRAW_REPLAY_BEATMAP_MATCHER_H

#include "beatmap_reader.h"
#include "bindable.h"
#include "replay_loader.h"

class Replay_beatmap_matcher : public Component {
public:
    Replay_beatmap_matcher(Bindable<std::optional<osu::Replay>>& replay, Beatmap_reader& beatmap_reader,
                           Api_manager& api);

private:
    void update(const Bindable<std::optional<osu::Replay>>& replay);

    Beatmap_reader& beatmap_reader;
    Api_manager& api;
};


#endif//SLIDER_DRAW_REPLAY_BEATMAP_MATCHER_H
