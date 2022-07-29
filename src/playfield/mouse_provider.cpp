#include "mouse_provider.h"

osu::Vector2 Mouse_provider::get_coordinates() const
{
    return coordinates;
}

void Mouse_provider::set_coordinates(osu::Vector2 coords)
{
    coordinates = coords;
}

bool Mouse_provider::was_clicked() const
{
    return clicked;
}

void Mouse_provider::set_was_clicked(bool clicked)
{
    this->clicked = clicked;
}

bool Mouse_provider::is_down() const
{
    return mouse_down;
}

void Mouse_provider::set_is_down(bool down)
{
    mouse_down = down;
}
