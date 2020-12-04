#include "coordinate_converter.h"
#include <Magnum/GL/Buffer.h>
#include <Magnum/Math/Matrix4.h>

void Coordinate_holder::set_resolution(const Magnum::Vector2i res)
{
    Magnum::Matrix4 converter = Magnum::Matrix4::perspectiveProjection(
            {static_cast<float>(res.x()), 0}, {0, static_cast<float>(res.y())},
            -1.f, 1.f);
    current_resolution = res;

    projection_buffer.setData({converter.data(), sizeof(converter)});
    projection_buffer.bind(Magnum::GL::Buffer::Target::Uniform, 0);
}

Magnum::Vector2i Coordinate_holder::resolution() const
{
    return current_resolution;
}