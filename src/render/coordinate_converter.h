#pragma once

#include <Magnum/GL/Buffer.h>
#include <Magnum/Math/Vector2.h>

class Coordinate_holder {
public:
    void set_resolution(const Magnum::Vector2i resolution);
    Magnum::Vector2i resolution() const;

private:
    Magnum::Vector2i current_resolution;
    Magnum::GL::Buffer projection_buffer{Magnum::GL::Buffer::TargetHint::Uniform};
};
