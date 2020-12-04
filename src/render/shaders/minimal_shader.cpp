#include "minimal_shader.h"
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

Minimal_shader::Minimal_shader()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GLES300);

    const Corrade::Utility::Resource rs{"shaders"};

    Magnum::GL::Shader vert{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Vertex};
    Magnum::GL::Shader frag{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Fragment};

    vert.addSource(rs.get("minimal.vert"));
    frag.addSource(rs.get("minimal.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    bindAttributeLocation(Position::Location, "position");
    bindAttributeLocation(Color::Location, "color");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    setUniformBlockBinding(uniformBlockIndex("projection"), 0);
}
