#include "slider.h"
#include "string_helpers.h"

std::optional<Slider> bezier_from_points(const std::vector<Slider_point>& points);

std::optional<Slider> bezier_from_points(const std::vector<Slider_point>& points){  // Todo: bounds check?
    Slider slider;
    slider.emplace_back();

    auto last_point = points.front();
    slider.back().push_back(last_point);
    for(std::size_t i = 1; i < points.size(); ++i){
        const auto current_point = points[i];
        if(last_point == current_point){
            slider.push_back(Slider_segment{ current_point });
        }
        else{
            slider.back().push_back(current_point);
            last_point = current_point;
        }
    }
    return slider;
}

std::optional<Slider> parse_slider(const std::string_view slider_string)
{
    const auto object_tokens = split(slider_string, ',');
    if(object_tokens.size() < 6) return std::nullopt;

    const auto slider_tokens = split(object_tokens[5], '|');
    if(slider_tokens.size() < 2) return std::nullopt;

    const auto type = static_cast<Slider_type>(slider_tokens[0].front());    //Todo: bounds checks everywhere

    std::vector<Slider_point> points = { { std::stof(object_tokens[0].data()), std::stof(object_tokens[1].data()) } };

    for(std::size_t i = 1; i < slider_tokens.size(); ++i){
        const auto token = slider_tokens[i];
        const auto coords = split(token, ':');
        points.emplace_back(std::stof(coords[0].data()), std::stof(coords[1].data()));
    }

    switch (type)
    {
    case Slider_type::bezier:
        return bezier_from_points(points);
    case Slider_type::perfect:
        return bezier_from_points(points);
    case Slider_type::linear:
        return bezier_from_points(points);
    case Slider_type::catmull:
        return bezier_from_points(points);
    
    default:
        return std::nullopt;
    }

    return Slider{points};
}