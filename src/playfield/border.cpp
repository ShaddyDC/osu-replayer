#include "border.h"
Playfield_border::Playfield_border(const Magnum::Vector2i& top_left, const Magnum::Vector2i& bottom_right)
    : top_left{top_left}, bottom_right{bottom_right}
{
    constexpr const auto border_width = 5.f;
    std::vector<Magnum::Vector2> points{
            //Todo: Fix top left corner
            Magnum::Vector2{top_left},
            Magnum::Vector2{static_cast<float>(bottom_right.x()), static_cast<float>(top_left.y())},
            Magnum::Vector2{bottom_right},
            Magnum::Vector2{static_cast<float>(top_left.x()), static_cast<float>(bottom_right.y())},
            Magnum::Vector2{top_left},
    };

    border_mesh = line_renderer.generate_mesh(points, border_width, {Magnum::Color4{1}});
}

void Playfield_border::draw(Magnum::GL::Framebuffer& target)
{
    target.bind();
    line_renderer.draw(border_mesh);
}
