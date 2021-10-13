#include "play_container.h"

#include "render/drawable_circle.h"
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
#include <osu_reader/beatmap_util.h>
#include <render/drawable_slider.h>

Play_container::Play_container(Api_manager& api_manager) : data{api_manager}, replay_container{api_manager}
{
    std::vector<Magnum::Vector2> points{
            //Todo: Fix top left corner
            Magnum::Vector2{top_left},
            Magnum::Vector2{static_cast<float>(bottom_right.x()), static_cast<float>(top_left.y())},
            Magnum::Vector2{bottom_right},
            Magnum::Vector2{static_cast<float>(top_left.x()), static_cast<float>(bottom_right.y())},
            Magnum::Vector2{top_left},
    };

    border_mesh = line_renderer.generate_mesh(points, 5.f, {Magnum::Color4{1}});
}

void Play_container::update(std::chrono::milliseconds time_passed)
{
    if(paused) time_passed = std::chrono::milliseconds::zero();
    if(last_time == current_time && time_passed == std::chrono::milliseconds::zero()) return;

    // Size stuff
    scaling_size = static_cast<Magnum::Vector2i>(size_scale * size);

    // Hitobject stuff
    current_time += duration_cast<std::chrono::milliseconds>(speed * time_passed);//Todo: Handle fractions better
    if(current_time.count() >= data.time_range().y().count()) current_time = std::chrono::milliseconds::zero();


    drawables.clear();
    Drawables approach_circles{};

    const auto add_approach_circle = [this, &approach_circles](auto pos, auto time) {
        const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(data.map->ar))};
        const auto early_duration = time - current_time;
        const auto progress = static_cast<float>(early_duration.count()) / early_window.count();
        const auto radius = osu::cs_to_osupixel(data.map->cs) * (1.f + 2 * progress);
        approach_circles.push_back(std::make_unique<Drawable_circle>(circle_renderer, pos, radius, Circle_draw_options{.circle_center = Circleobject_shader::hollow}));
    };

    const auto draw_circle = [this, &add_approach_circle](auto& circle) {
        const auto position = to_screen(circle.position);

        drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer, position, osu::cs_to_osupixel(data.map->cs), Circle_draw_options{}));
        if(circle.time > current_time) add_approach_circle(position, circle.time);
    };

    const auto draw_slider = [this, &add_approach_circle](auto& slider) {
        if(slider.time > current_time) add_approach_circle(to_screen(vector_o2m(slider.slider.points.front())), slider.time);
        drawables.push_back(std::make_unique<Drawable_slider>(slider_renderer, circle_renderer, slider, static_cast<Magnum::Vector2>(top_left),
                                                              osu::cs_to_osupixel(data.map->cs), current_time));
    };

    auto circles = data.circles_at(current_time);
    auto sliders = data.sliders_at(current_time);

    auto circles_it = circles.begin();
    auto sliders_it = sliders.begin();

    while(circles_it != circles.end() || sliders_it != sliders.end()) {
        if(circles_it == circles.end() || (sliders_it != sliders.end() && circles_it->time >= sliders_it->time)) draw_slider(*sliders_it++);
        else
            draw_circle(*circles_it++);
    }

    std::reverse(drawables.begin(), drawables.end());
    drawables.insert(drawables.end(), std::make_move_iterator(approach_circles.begin()), std::make_move_iterator(approach_circles.end()));

    if(replay_container.replay) {
        // TODO This is incorrect, not the replay but the map is flipped
        const auto current_frame = replay_container.frame_at(current_time);
        auto pos = Magnum::Vector2{current_frame.x, current_frame.y};
        if((replay_container.replay->mods & 16) > 0) {
            pos.y() = 384.f - pos.y();
        }
        pos = to_screen(pos);
        drawables.push_back(std::make_unique<Drawable_circle>(circle_renderer,
                                                              pos,
                                                              osu::cs_to_osupixel(data.map->cs) / 5,
                                                              Circle_draw_options{
                                                                      .color = Magnum::Color4::yellow(),
                                                                      .circle_center = Circleobject_shader::filled}));
    }

    last_time = current_time;
}

Magnum::GL::Texture2D Play_container::draw()
{
    // Prepare Texture
    Magnum::GL::Texture2D texture;
    texture.setStorage(1, Magnum::GL::TextureFormat::RGBA8, scaling_size);

    Magnum::GL::Framebuffer framebuffer{{{}, scaling_size}};
    framebuffer.bind();
    framebuffer.attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0}, texture, 0);

    framebuffer.clearColor(0, Magnum::Color4{0, 0, 0, 0});
    framebuffer.clear(Magnum::GL::FramebufferClear::Depth).bind();

    //Todo: Better to set viewport accordingly
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);

    line_renderer.draw(border_mesh);

    for(auto& drawable : drawables) drawable->draw(framebuffer);

    //    auto slider_it = sliders.rbegin();
    //    auto circle_it = circles.rbegin();
    //    auto follow_it = slider_follow_circles.rbegin();
    //
    //    while(slider_it != sliders.rend() || circle_it != circles.rend()) {
    //        const auto draw_circle = [&]() {
    //            circle_renderer.draw(circle_it->mesh);
    //            ++circle_it;
    //        };
    //        const auto draw_slider = [&]() {
    //            slider_renderer.draw(slider_it->mesh, framebuffer);
    //
    //            while(follow_it != slider_follow_circles.rend() && follow_it->time == slider_it->time) {// TODO: Bad check
    //                circle_renderer.draw(follow_it->mesh, {.circle_center = Circleobject_shader::hollow});
    //                ++follow_it;
    //            }
    //
    //            ++slider_it;
    //        };
    //
    //        if(slider_it == sliders.rend()) draw_circle();
    //        else if(circle_it == circles.rend())
    //            draw_slider();
    //        else {
    //            if(circle_it->time.count() > slider_it->time.count()) draw_circle();
    //            else
    //                draw_slider();
    //        }
    //    }
    //
    //    for(auto it = approach_circles.rbegin(); it != approach_circles.rend(); ++it) {
    //        circle_renderer.draw(it->mesh, {.circle_center = Circleobject_shader::hollow});
    //    }

    Magnum::GL::defaultFramebuffer.bind();

    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);

    return texture;
}

Magnum::Vector2 Play_container::to_screen(const Magnum::Vector2 point)
{
    return point + static_cast<Magnum::Vector2>(top_left);
}
