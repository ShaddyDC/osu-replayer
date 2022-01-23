#ifndef SLIDER_DRAW_BORDER_H
#define SLIDER_DRAW_BORDER_H

#include "render/line_renderer.h"
#include "render/drawable.h"
#include <Magnum/GL/Mesh.h>

class Playfield_border : public Drawable {
public:
    Playfield_border(const Magnum::Vector2i& top_left, const Magnum::Vector2i& bottom_right);

    void draw(Magnum::GL::Framebuffer& target) override;


private:
    Line_mesh border_mesh;
    Line_renderer line_renderer;
    const Magnum::Vector2i& top_left;
    const Magnum::Vector2i& bottom_right;
};


#endif//SLIDER_DRAW_BORDER_H
