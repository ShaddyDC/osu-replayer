#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>

class Sliderbody_shader : public Magnum::GL::AbstractShaderProgram {
public:
    using Position = Magnum::GL::Attribute<0, Magnum::Vector3>;
    using Side = Magnum::GL::Attribute<1, Magnum::Float>;

    explicit Sliderbody_shader();
};
