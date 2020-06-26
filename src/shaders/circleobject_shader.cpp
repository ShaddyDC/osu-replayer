#include "circleobject_shader.h"
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/Reference.h>

Circleobject_shader::Circleobject_shader()
{
	MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GLES300);

	const Corrade::Utility::Resource rs{ "shaders" };

	Magnum::GL::Shader vert{ Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Vertex };
	Magnum::GL::Shader frag{ Magnum::GL::Version::GLES300, Magnum::GL::Shader::Type::Fragment };

	vert.addSource(rs.get("circleobject.vert"));
	frag.addSource(rs.get("circleobject.frag"));

	CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({ vert, frag }));

	attachShaders({ vert, frag });

    bindAttributeLocation(Position::Location, "position");
    bindAttributeLocation(Local_position::Location, "vLocalPos");

	CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
