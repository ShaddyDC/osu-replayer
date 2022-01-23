#ifndef SLIDER_DRAW_COMPONENT_H
#define SLIDER_DRAW_COMPONENT_H

#include <memory>
#include <vector>
#include <chrono>

class Component {
public:
    Component() = default;
    Component(const Component&) = default;
    Component(Component&&) = default;
    Component& operator=(const Component&) = default;
    Component& operator=(Component&&) = default;
    virtual ~Component() = default;

    virtual void update(std::chrono::milliseconds time_passed) { (void) time_passed; };
    virtual void draw() = 0;
};

using Container = std::vector<std::unique_ptr<Component>>;

#endif//SLIDER_DRAW_COMPONENT_H
