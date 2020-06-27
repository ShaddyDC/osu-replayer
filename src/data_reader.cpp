#include "data_reader.h"
#include <Corrade/Utility/Resource.h>
#include <string>
#include "string_helpers.h"
#include <algorithm>

enum Hitobject_type : char{
	circle = 1,
	slider = 2,
	spinner = 8,
};

bool is_type(const int a, const Hitobject_type b)
{
	return (a & static_cast<int>(b)) > 0;
}

Data_reader::Data_reader()
{
	const Corrade::Utility::Resource rs{ "data" };

	const auto data = rs.get("example.txt");

	for(const auto line : split(data, '\n')){
		const auto tokens = split(line, ',');
		if(tokens.size() < 4) continue;

		const auto time = std::stoi(tokens[2].data());
		const auto type = std::stoi(tokens[3].data());
		if(is_type(type, Hitobject_type::circle)){
			circles.emplace_back(Magnum::Math::Vector2<float>{ std::stof(tokens[0].data()), std::stof(tokens[1].data()) }, time);
		} 
		else if(is_type(type, Hitobject_type::slider)){
			sliders.emplace_back(parse_slider(line).value(), time);
		}
		else if(is_type(type, Hitobject_type::spinner)){
			Corrade::Utility::Debug() << "Ignore Spiner " << line << '\n';
		}
	}
}

std::vector<Circle_object> Data_reader::circles_at(int time)
{
	std::vector<Circle_object> ret;

	const auto in_time = [time](const auto& obj)
		{
			return std::abs(time - obj.time) < 1'000;
		};

	std::copy_if(circles.cbegin(), circles.cend(), std::back_inserter(ret), in_time);
	return ret;
}

std::vector<Slider_object> Data_reader::sliders_at(int time)
{
	std::vector<Slider_object> ret;

	const auto in_time = [time](const auto& obj)
		{
			return std::abs(time - obj.time) < 1'000;
		};

	std::copy_if(sliders.cbegin(), sliders.cend(), std::back_inserter(ret), in_time);
	return ret;
}


Magnum::Math::Vector2<int> Data_reader::time_range()
{
	const int start = std::min(sliders.front().time, circles.front().time);	//Todo: Bounds Check
	const int end = std::max(sliders.back().time, circles.back().time);
	return { start - 5, end + 5 };
}