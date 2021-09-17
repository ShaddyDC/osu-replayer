#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>

class Circleobject_shader : public Magnum::GL::AbstractShaderProgram {
public:
    using Position = Magnum::GL::Attribute<0, Magnum::Vector2>;
    using Local_position = Magnum::GL::Attribute<1, Magnum::Vector2>;

    enum Circle_center : Magnum::UnsignedInt {
        gradient,
        filled,
        hollow
    };

    explicit Circleobject_shader();

    void set_color(Magnum::Color4 color);
    void set_circle_center(Circle_center center);

private:
    Magnum::Int color_uniform;
    Magnum::Int circle_center_uniform;
};
