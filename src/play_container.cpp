#include "play_container.h"

#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/DefaultFramebuffer.h>



Play_container::Play_container()
{
    std::vector<Magnum::Vector2> points{    //Todo: Fix top left corner
        Magnum::Vector2{ top_left },
        Magnum::Vector2{ static_cast<float>(bottom_right.x()), static_cast<float>(top_left.y()) },
        Magnum::Vector2{ bottom_right },
        Magnum::Vector2{ static_cast<float>(top_left.x()), static_cast<float>(bottom_right.y()) },
        Magnum::Vector2{ top_left },
    };

    // std::vector<Magnum::Vector2> points{    //Todo: Fix top left corner
    //     Magnum::Vector2{ -1, -1 },
    //     Magnum::Vector2{ 1, -1 },
    //     Magnum::Vector2{ 1, 1 },
    //     Magnum::Vector2{ -1, 1 },
    //     Magnum::Vector2{ -1, -1 },
    // };
    border_mesh = line_renderer.generate_mesh(points, 5.f, { Magnum::Color4{ 1 } });
}

void Play_container::update(int time_passed)
{
    if(paused) time_passed = 0;
    if(last_time == current_time && time_passed == 0) return;

    // Size stuff
    // Todo: Handle scissoring
    scaling_size = static_cast<Magnum::Vector2i>(size_scale * size);

    // Hitobject stuff
    current_time += static_cast<int>(speed * time_passed);      //Todo: Handle fractions better
    if(current_time >= data.time_range().y()) current_time = 0;

    circles.clear();
    sliders.clear();

    for (auto circle : data.circles_at(current_time)){
        const auto position = to_screen(circle.position);

        circles.emplace_back(circle_renderer.generate_mesh(position, 30.f), circle.time);
    }

    for (auto slider : data.sliders_at(current_time)){
        for(auto& segment : slider.slider){
            for(auto& point : segment){
                point = to_screen(point);
            }
        }
        sliders.emplace_back(slider_renderer.generate_mesh(slider.slider, 30.f), slider.time);
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

    framebuffer.clearColor(0, Magnum::Color4{ 0, 0, 0, 0 });
    framebuffer.clear(Magnum::GL::FramebufferClear::Depth).bind();

    line_renderer.draw(border_mesh);

    auto slider_it = sliders.rbegin();
    auto circle_it = circles.rbegin();

    while(slider_it != sliders.rend() || circle_it != circles.rend()){
        const auto draw_circle = [&]()
            {
                circle_renderer.draw(circle_it->mesh);
                ++circle_it;
            };
        const auto draw_slider = [&]()
            {
                slider_renderer.draw(slider_it->mesh, framebuffer);
                ++slider_it;
            };
            
        if(slider_it == sliders.rend()) draw_circle();
        else if(circle_it == circles.rend()) draw_slider();
        else{
            if(circle_it->time > slider_it->time) draw_circle();
            else draw_slider();
        }
    }

    Magnum::GL::defaultFramebuffer.bind();

    return texture;
}

Magnum::Vector2 Play_container::to_screen(const Magnum::Vector2 point)
{
    return point + static_cast<Magnum::Vector2>(top_left);
}
