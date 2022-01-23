#pragma once

#include "circleobject_renderer.h"
#include "drawable.h"

class Drawable_circle : public Drawable {
public:
    Drawable_circle(Circleobject_renderer& renderer, Magnum::Vector2 position, float radius, Circle_draw_options options)
        : renderer{renderer}, position{position}, radius{radius}, options{options} {}

    void draw(Magnum::GL::Framebuffer& target) override
    {
        target.bind();
        auto mesh = renderer.generate_mesh(position, radius);
        renderer.draw(mesh, options);
    }

private:
    Circleobject_renderer& renderer;
    Magnum::Vector2 position;
    float radius;
    Circle_draw_options options;
};