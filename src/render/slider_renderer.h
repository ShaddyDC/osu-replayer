#pragma once
#include "../slider.h"
#include "circleobject_renderer.h"
#include "shaders/sliderbody_shader.h"
#include "vertex_generate.h"
#include <Magnum/GL/Framebuffer.h>// TODO: Investigate potentially fewer includes
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>


using Sliderbody_mesh = Magnum::GL::Mesh;

struct Slider_mesh {
    Circleobject_mesh head;
    Sliderbody_mesh body;
};

class Slider_renderer {
public:
    Slider_mesh generate_mesh(const Slider& slider, const float radius);
    void draw(Slider_mesh& mesh, Magnum::GL::Framebuffer& target);
    void draw(Sliderbody_mesh& mesh);

private:
    Sliderbody_shader shader;
    Circleobject_renderer circle_renderer;
    Magnum::Shaders::Flat2D flat_shader{Magnum::Shaders::Flat2D::Flag::Textured};
};
