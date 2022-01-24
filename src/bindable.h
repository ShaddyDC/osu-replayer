#ifndef SLIDER_DRAW_BINDABLE_H
#define SLIDER_DRAW_BINDABLE_H

#include <functional>
#include <utility>
#include <vector>

template<typename Type>
class Bindable {
public:
    explicit Bindable(Type v) : v{v} {}
    void set(Type value)
    {
        v = std::move(value);
        callback();
    }
    const Type& get() const { return v; }

private:
    void callback()
    {
        for(auto& f : callbacks) f(*this);
    }
    Type v;
    std::vector<std::function<void(Bindable<Type>&)>> callbacks;
};

#endif//SLIDER_DRAW_BINDABLE_H
