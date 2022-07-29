#ifndef OSU_REPLAYER_MOUSE_PROVIDER_H
#define OSU_REPLAYER_MOUSE_PROVIDER_H

#include "osu_reader/vector2.h"

class Mouse_provider {
public:
    Mouse_provider() = default;

    [[nodiscard]] osu::Vector2 get_coordinates() const;
    void set_coordinates(osu::Vector2 coords);
    [[nodiscard]] bool was_clicked() const;
    void set_was_clicked(bool clicked);
    [[nodiscard]] bool is_down() const;
    void set_is_down(bool down);

    // Immovable so can be passed per reference
    Mouse_provider(const Mouse_provider&) = delete;
    Mouse_provider(Mouse_provider&&) = delete;
    const Mouse_provider& operator=(const Mouse_provider&) = delete;
    const Mouse_provider& operator=(Mouse_provider&&) = delete;

private:
    osu::Vector2 coordinates;
    bool clicked;
    bool mouse_down;
};


#endif//OSU_REPLAYER_MOUSE_PROVIDER_H
