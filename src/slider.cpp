#include "slider.h"
#include <algorithm>
#include <string>
enum class Slider_type : char{
    bezier = 'B',
    catmull = 'C',
    perfect = 'P',
    linear = 'L'
};
std::vector<std::string_view> split(const std::string_view s, const char delim);
std::optional<Slider> bezier_from_points(const std::vector<Slider_point>& points);

std::vector<std::string_view> split(const std::string_view s, const char delim)
{
	std::vector<std::string_view> ret;
	auto it       = std::find(s.begin(), s.end(), delim);
	auto it_start = s.begin();

	const auto add_if_not_empty = [&]
	{
		if(it != it_start){
			ret.emplace_back(s.data() + (it_start - s.begin()), it - it_start);
		}
	};

	for(; it != s.end(); it = std::find(it, s.end(), delim)){
        add_if_not_empty();
        it_start = it = it + 1;
	}
	add_if_not_empty();
	return ret;
}

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
    
    default:
        return std::nullopt;
    }

    return Slider{points};
}