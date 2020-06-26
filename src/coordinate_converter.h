#pragma once

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>  //Todo: Remove this

class Coordinate_converter;

class Coordinate_holder {
public:
    Coordinate_holder() = default;

    void set_resolution(const Magnum::Vector2i resolution);
    Magnum::Vector2 to_screen(const Magnum::Vector2 point) const;
    Magnum::Vector2i resolution() const;

    operator Coordinate_converter() const;

    // Held as const reference, so make sure it's not moved
    Coordinate_holder(const Coordinate_holder&) = delete;
    Coordinate_holder(Coordinate_holder&&) = delete;
    const Coordinate_holder& operator=(const Coordinate_holder&) = delete;
    const Coordinate_holder& operator=(Coordinate_holder&&) = delete;
    
private:
    Magnum::Vector2i current_resolution;
    Magnum::Matrix4 converter;
};

class Coordinate_converter{
public:
    Coordinate_converter(const Coordinate_holder& holder);
    Magnum::Vector2 to_screen(const Magnum::Vector2 point) const;
    Magnum::Vector2i resolution() const;

private:
    // const ref is okay because Coordinate_holder cannot be moved
    const Coordinate_holder& holder;
};
