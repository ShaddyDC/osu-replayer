#ifndef SLIDER_DRAW_PLAYFIELD_SIZE_MANAGER_H
#define SLIDER_DRAW_PLAYFIELD_SIZE_MANAGER_H

#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector2.h>

#include "component.h"
class Playfield_size_manager : public Component {
public:
    void update(std::chrono::milliseconds time_passed) override;

    [[nodiscard]] Magnum::Vector2i get_size() const;
    [[nodiscard]] Magnum::Vector2i get_internal_size() const;
    [[nodiscard]] Magnum::Vector2i get_top_left() const;
    [[nodiscard]] Magnum::Vector2i get_bottom_right() const;
    [[nodiscard]] float get_scale() const;

private:
    const Magnum::Vector2i field_size = {512, 384};
    const Magnum::Vector2i top_left{50, 50};
    const Magnum::Vector2i bottom_right{top_left + field_size};
    const Magnum::Vector2i size_unscaled = {bottom_right + top_left};
    float scale = 1.f;
    Magnum::Vector2i scaling_size = size_unscaled;
};


#endif//SLIDER_DRAW_PLAYFIELD_SIZE_MANAGER_H
