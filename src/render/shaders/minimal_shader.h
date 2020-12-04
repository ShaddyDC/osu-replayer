#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>

class Minimal_shader : public Magnum::GL::AbstractShaderProgram {
public:
    using Position = Magnum::GL::Attribute<0, Magnum::Vector2>;
    using Color = Magnum::GL::Attribute<1, Magnum::Vector4>;

    explicit Minimal_shader();
};
