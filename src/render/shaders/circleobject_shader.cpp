#include "circleobject_shader.h"
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>

Circleobject_shader::Circleobject_shader()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GLES300);

    const Corrade::Utility::Resource rs{"shaders"};

    Magnum::GL::Shader vert{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Vertex};
    Magnum::GL::Shader frag{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Fragment};

    vert.addSource(rs.get("circleobject.vert"));
    frag.addSource(rs.get("circleobject.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    bindAttributeLocation(Position::Location, "position");
    bindAttributeLocation(Local_position::Location, "vLocalPos");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    setUniformBlockBinding(uniformBlockIndex("projection"), 0);

    color_uniform = uniformLocation("circleColor");
    circle_center_uniform = uniformLocation("circleCenter");
}

void Circleobject_shader::set_color(Magnum::Color4 color)
{
    setUniform(color_uniform, color);
}
void Circleobject_shader::set_circle_center(Circle_center center)
{
    setUniform(circle_center_uniform, (Magnum::UnsignedInt) center);
}
