#include "coordinate_converter.h"

void Coordinate_holder::set_resolution(const Magnum::Vector2i res)
{
    converter = Magnum::Matrix4::perspectiveProjection(
        { static_cast<float>(res.x()), 0 }, { 0, static_cast<float>(res.y()) },
         -1.f, 1.f);
    current_resolution = res;
}

Magnum::Vector2 Coordinate_holder::to_screen(const Magnum::Vector2 point) const
{   
    return (converter * Magnum::Vector4{ point.x(), point.y(), 1, 0 }).xy();
}

Magnum::Vector2i Coordinate_holder::resolution() const
{
    return current_resolution;
}

Coordinate_holder::operator Coordinate_converter() const
{
    return Coordinate_converter{ *this };
}

Coordinate_converter::Coordinate_converter(const Coordinate_holder& holder)
: holder{ holder }
{}

Magnum::Vector2 Coordinate_converter::to_screen(const Magnum::Vector2 point) const
{
    return holder.to_screen(point);
}

Magnum::Vector2i Coordinate_converter::resolution() const
{
    return holder.resolution();
}
