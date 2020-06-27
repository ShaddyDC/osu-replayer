#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>

class Circleobject_shader : public Magnum::GL::AbstractShaderProgram {
public:
	using Position = Magnum::GL::Attribute<0, Magnum::Vector2>;
	using Local_position = Magnum::GL::Attribute<1, Magnum::Vector2>;

	explicit Circleobject_shader();
};
