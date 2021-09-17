#include "play_container.h"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>
#include <osu_reader/beatmap_util.h>

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

    circles.clear();
    sliders.clear();
    approach_circles.clear();
    slider_follow_circles.clear();

    const auto add_approach_circle = [this](auto pos, auto time) {
        const auto early_window = std::chrono::milliseconds{static_cast<int>(osu::ar_to_ms(data.map->ar))};
        const auto early_duration = time - current_time;
        const auto progress = static_cast<float>(early_duration.count()) / early_window.count();
        const auto radius = osu::cs_to_osupixel(data.map->cs) * (1.f + 2 * progress);
        approach_circles.emplace_back(Circleobject_renderer::generate_mesh(pos, radius), time);
    };

    for(auto circle : data.circles_at(current_time)) {
        const auto position = to_screen(circle.position);

        circles.emplace_back(Circleobject_renderer::generate_mesh(position, osu::cs_to_osupixel(data.map->cs)), circle.time);
        if(circle.time > current_time) add_approach_circle(position, circle.time);
    }

    for(auto slider : data.sliders_at(current_time)) {
        for(auto& point : slider.slider.points) {
            const auto p = to_screen({point.x, point.y});
            point.x = p.x();
            point.y = p.y();
        }
        sliders.emplace_back(slider_renderer.generate_mesh(slider.slider, osu::cs_to_osupixel(data.map->cs)), slider.time);
        if(slider.time > current_time) add_approach_circle(Magnum::Vector2{slider.slider.points.front().x, slider.slider.points.front().y}, slider.time);

        //        if(slider.time + slider.slider.duration >= current_time)
        {// Follow circle
            const auto radius = osu::cs_to_osupixel(data.map->cs) * 2.4f;

            const auto progress = static_cast<float>((current_time - slider.time).count()) / slider.slider.duration.count();
            const auto internal_progress = std::clamp(progress, 0.f, 1.f);
            const auto extended_progress = (internal_progress * slider.slider.repeat);
            const auto current_repeat = static_cast<int>(extended_progress);
            const auto uneven_repeat = current_repeat % 2;

            if(progress >= 0 && progress <= 1) {
                const auto current_progress = extended_progress - current_repeat;
                const auto direction_progress = uneven_repeat == 0 ? current_progress : 1 - current_progress;
                const auto pixel_progress = direction_progress * slider.slider.length;
                const auto current_pos_it = std::find_if(slider.slider.distances.cbegin(), slider.slider.distances.cend(),
                                                         [pixel_progress](const auto dist) { return dist >= pixel_progress; });
                auto pos = slider.slider.points.front();
                if(current_pos_it != slider.slider.distances.cend()) {
                    const auto pos_index = std::distance(slider.slider.distances.cbegin(), current_pos_it);
                    pos = slider.slider.points[pos_index];
                    if(current_pos_it != slider.slider.distances.cbegin() && *current_pos_it != pixel_progress) {
                        const auto lerp_value = (pixel_progress - *(current_pos_it - 1)) / (*current_pos_it - *(current_pos_it - 1));
                        pos = lerp(slider.slider.points[pos_index - 1], slider.slider.points[pos_index], lerp_value);
                    }
                }

                slider_follow_circles.emplace_back(Circleobject_renderer::generate_mesh({pos.x, pos.y}, radius), slider.time);
            }

            if(current_repeat < slider.slider.repeat - 1) {
                const auto repeat_point_radius = osu::cs_to_osupixel(data.map->cs) / 3;

                auto pos = uneven_repeat == 0 ? slider.slider.points.back() : slider.slider.points.front();
                slider_follow_circles.emplace_back(Circleobject_renderer::generate_mesh({pos.x, pos.y}, repeat_point_radius), slider.time);

                if(current_repeat < slider.slider.repeat - 2) {
                    pos = uneven_repeat == 0 ? slider.slider.points.front() : slider.slider.points.back();
                    slider_follow_circles.emplace_back(Circleobject_renderer::generate_mesh({pos.x, pos.y}, repeat_point_radius), slider.time);
                }
            }
        }
    }

    std::sort(approach_circles.begin(), approach_circles.end(), [](const auto& a, const auto& b) { return a.time < b.time; });

    if(replay_container.replay) {
        // TODO This is incorrect, not the replay but the map is flipped
        const auto current_frame = replay_container.frame_at(current_time);
        auto pos = Magnum::Vector2{current_frame.x, current_frame.y};
        if((replay_container.replay->mods & 16) > 0) {
            pos.y() = 384.f - pos.y();
        }
        pos = to_screen(pos);
        current_cursor = Circleobject_renderer::generate_mesh(pos, osu::cs_to_osupixel(data.map->cs) / 5);
    }

    last_time = current_time;
}

Magnum::GL::Texture2D Play_container::draw()
{
    // Prepare Texture
    Magnum::GL::Texture2D texture;
    texture.setStorage(1, Magnum::GL::TextureFormat::RGBA8, scaling_size);

    Magnum::GL::Framebuffer framebuffer{{{}, scaling_size}};
    framebuffer.attachTexture(Magnum::GL::Framebuffer::ColorAttachment{0}, texture, 0);

    framebuffer.clearColor(0, Magnum::Color4{0, 0, 0, 0});
    framebuffer.clear(Magnum::GL::FramebufferClear::Depth).bind();

    //Todo: Better to set viewport accordingly
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);

    line_renderer.draw(border_mesh);

    auto slider_it = sliders.rbegin();
    auto circle_it = circles.rbegin();
    auto follow_it = slider_follow_circles.rbegin();

    while(slider_it != sliders.rend() || circle_it != circles.rend()) {
        const auto draw_circle = [&]() {
            circle_renderer.draw(circle_it->mesh);
            ++circle_it;
        };
        const auto draw_slider = [&]() {
            slider_renderer.draw(slider_it->mesh, framebuffer);

            while(follow_it != slider_follow_circles.rend() && follow_it->time == slider_it->time) {// TODO: Bad check
                circle_renderer.draw(follow_it->mesh, {.circle_center = Circleobject_shader::hollow});
                ++follow_it;
            }

            ++slider_it;
        };

        if(slider_it == sliders.rend()) draw_circle();
        else if(circle_it == circles.rend())
            draw_slider();
        else {
            if(circle_it->time.count() > slider_it->time.count()) draw_circle();
            else
                draw_slider();
        }
    }

    for(auto it = approach_circles.rbegin(); it != approach_circles.rend(); ++it) {
        circle_renderer.draw(it->mesh, {.circle_center = Circleobject_shader::hollow});
    }

    if(current_cursor)
        circle_renderer.draw(*current_cursor, {.color = Magnum::Color4::yellow(), .circle_center = Circleobject_shader::filled});

    Magnum::GL::defaultFramebuffer.bind();

    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);

    return texture;
}

Magnum::Vector2 Play_container::to_screen(const Magnum::Vector2 point)
{
    return point + static_cast<Magnum::Vector2>(top_left);
}
