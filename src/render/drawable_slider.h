#pragma once

#include "data/hitobjects.h"
#include "drawable.h"
#include "drawable_circle.h"
#include "playfield/playfield_coordinate_provider.h"
#include "slider_renderer.h"
#include "util/vector_util.h"
#include <Magnum/GL/Texture.h>

class Drawable_slider : public Drawable {
public:
    Drawable_slider(Slider_renderer& renderer, Circleobject_renderer& circle_renderer, const Slider_object& slider, bool is_selected, const Playfield_coordinate_provider& coordinate_provider, float radius, std::chrono::milliseconds current_time)
        : renderer{renderer}
    {
        auto slider_copy = slider;
        for(auto& point : slider_copy.slider.points) {
            point = vector_m2o(coordinate_provider.osu_to_field(vector_o2m(point)));
        }
        texture = renderer.generate_texture(slider_copy.slider, radius, is_selected);


        //        if(slider.time + slider.slider.duration >= current_time)
        {
            // Follow circle
            const auto follow_radius = radius * 2.4f;

            const auto progress = static_cast<float>((current_time - slider_copy.time).count()) / static_cast<float>(slider_copy.slider.duration.count());
            const auto internal_progress = std::clamp(progress, 0.f, 1.f);
            const auto extended_progress = (internal_progress * static_cast<float>(slider_copy.slider.repeat));
            const auto current_repeat = static_cast<int>(extended_progress);
            const auto uneven_repeat = current_repeat % 2;

            if(progress >= 0 && progress <= 1) {
                const auto current_progress = extended_progress - static_cast<float>(current_repeat);
                const auto direction_progress = uneven_repeat == 0 ? current_progress : 1 - current_progress;
                const auto pixel_progress = direction_progress * slider_copy.slider.length;
                const auto current_pos_it = std::find_if(slider_copy.slider.distances.cbegin(), slider_copy.slider.distances.cend(),
                                                         [pixel_progress](const auto dist) { return dist >= pixel_progress; });
                auto pos = slider_copy.slider.points.front();
                if(current_pos_it != slider_copy.slider.distances.cend()) {
                    const auto pos_index = std::distance(slider_copy.slider.distances.cbegin(), current_pos_it);
                    pos = slider_copy.slider.points[pos_index];
                    if(current_pos_it != slider_copy.slider.distances.cbegin() && *current_pos_it != pixel_progress) {
                        const auto lerp_value = (pixel_progress - *(current_pos_it - 1)) / (*current_pos_it - *(current_pos_it - 1));
                        pos = lerp(slider_copy.slider.points[pos_index - 1], slider_copy.slider.points[pos_index], lerp_value);
                    }
                }

                drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, vector_o2m(pos), follow_radius, Circle_draw_options{.circle_center = Circleobject_shader::hollow}));
            }

            if(current_repeat < slider_copy.slider.repeat - 1) {
                const auto repeat_point_radius = radius / 3;

                auto pos = uneven_repeat == 0 ? slider_copy.slider.points.back() : slider_copy.slider.points.front();
                drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, vector_o2m(pos), repeat_point_radius, Circle_draw_options{.circle_center = Circleobject_shader::hollow}));

                if(current_repeat < slider_copy.slider.repeat - 2) {
                    pos = uneven_repeat == 0 ? slider_copy.slider.points.front() : slider_copy.slider.points.back();
                    drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, vector_o2m(pos), repeat_point_radius, Circle_draw_options{.circle_center = Circleobject_shader::hollow}));
                }
            }
        }

        if(is_selected) {
            const auto point_radius = radius / 10;
            for(const auto& p : slider_copy.slider.points) {
                drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, vector_o2m(p), point_radius, Circle_draw_options{.color = Magnum::Color4::red(), .circle_center = Circleobject_shader::filled}));
            }
        }
    }

    void draw(Magnum::GL::Framebuffer& target) override
    {
        renderer.draw(texture, target);
        for(auto& drawable : drawables) drawable->draw(target);
    }

private:
    Slider_renderer& renderer;
    Magnum::GL::Texture2D texture;
    Drawables drawables = {};
};