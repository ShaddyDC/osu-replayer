#ifndef SLIDER_DRAW_BINDABLE_H
#define SLIDER_DRAW_BINDABLE_H

#include <functional>
#include <utility>
#include <vector>

template<typename Type>
class Bindable {
public:
    explicit Bindable(Type v) : v{std::move(v)} {}
    void set(Type value)
    {
        v = std::move(value);
        callback();
    }
    [[nodiscard]] const Type& get() const { return v; }

    template<typename F>
    void register_callback(F f)
    {
        callbacks.template emplace_back(f);
    }

private:
    void callback()
    {
        for(auto& f : callbacks) f(*this);
    }
    Type v;
    std::vector<std::function<void(Bindable<Type>&)>> callbacks;
};

#endif//SLIDER_DRAW_BINDABLE_H
