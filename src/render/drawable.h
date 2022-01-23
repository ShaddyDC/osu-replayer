#pragma once

#include <Magnum/GL/Framebuffer.h>
#include <memory>
#include <vector>

class Drawable {
public:
    virtual void draw(Magnum::GL::Framebuffer& target) = 0;

    Drawable() = default;
    Drawable(const Drawable&) = default;
    Drawable(Drawable&&) = default;
    Drawable& operator=(const Drawable&) = default;
    Drawable& operator=(Drawable&&) = default;
    virtual ~Drawable() = default;
};

using Drawables = std::vector<std::unique_ptr<Drawable>>;
