#pragma once

#include <Magnum/GL/Framebuffer.h>

class Drawable {
public:
    virtual void draw(Magnum::GL::Framebuffer& target) = 0;
    virtual ~Drawable() = default;
};

using Drawables = std::vector<std::unique_ptr<Drawable>>;
