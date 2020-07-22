#pragma once
#include "shaders/sliderbody_shader.h"
#include "../slider.h"
#include "circleobject_renderer.h"
#include "vertex_generate.h"
#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Framebuffer.h>  // TODO: Investigate potentially fewer includes


using Sliderbody_mesh = Magnum::GL::Mesh;

struct Slider_mesh{
    Circleobject_mesh head;
    Sliderbody_mesh body;
};

class Slider_renderer{
public:
    Slider_mesh generate_mesh(const Slider& slider, const float radius);
    void draw(Slider_mesh& mesh, Magnum::GL::Framebuffer& target);
    void draw(Sliderbody_mesh& mesh);

private:
    Sliderbody_shader shader;
    Circleobject_renderer circle_renderer;
    Magnum::Shaders::Flat2D flat_shader{ Magnum::Shaders::Flat2D::Flag::Textured };
};
