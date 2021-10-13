#pragma once
#include "circleobject_renderer.h"
#include "shaders/sliderbody_shader.h"
#include "vertex_generate.h"
#include <Magnum/GL/Framebuffer.h>// TODO: Investigate potentially fewer includes
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/FlatGL.h>
#include <osu_reader/hitobject.h>


using Sliderbody_mesh = Magnum::GL::Mesh;

struct Slider_mesh {
    Circleobject_mesh head;
    Sliderbody_mesh body;
};

class Slider_renderer {
public:
    Magnum::GL::Texture2D generate_texture(const osu::Slider& slider, const float radius);
    void draw(Magnum::GL::Texture2D& texture, Magnum::GL::Framebuffer& target);
    void draw(Sliderbody_mesh& mesh);

private:
    Sliderbody_shader shader;
    Circleobject_renderer circle_renderer;
    Magnum::Shaders::FlatGL2D flat_shader{Magnum::Shaders::FlatGL2D::Flag::Textured};
};
