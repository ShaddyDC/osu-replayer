#include "sliderbody_shader.h"
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Attribute.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

Sliderbody_shader::Sliderbody_shader()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GLES300);

    const Corrade::Utility::Resource rs{"shaders"};

    Magnum::GL::Shader vert{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Vertex};
    Magnum::GL::Shader frag{Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Fragment};

    vert.addSource(rs.getString("sliderbody.vert"));
    frag.addSource(rs.getString("sliderbody.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

    attachShaders({vert, frag});

    bindAttributeLocation(Position::Location, "position");
    bindAttributeLocation(Side::Location, "side");


    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    setUniformBlockBinding(uniformBlockIndex("projection"), 0);
}
