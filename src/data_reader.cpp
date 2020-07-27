#include "data_reader.h"
#include <Corrade/Utility/Resource.h>
#include <Magnum/Magnum.h>
#include <string>
#include "string_helpers.h"
#include <algorithm>
#include <imgui.h>
#include "web_request.h"
#include <osu_reader/beatmap_util.h>

using namespace std::chrono_literals;

Data_reader::Data_reader()
{
	const Corrade::Utility::Resource rs{ "data" };

	map_string = rs.get("example.osu");

	init_map();
}

std::vector<Circle_object> Data_reader::circles_at(std::chrono::milliseconds time)
{
	std::vector<Circle_object> ret;

	const auto early_window = std::chrono::milliseconds{ static_cast<int>(osu::ar_to_ms(map->ar)) };
	const auto late_window = std::chrono::milliseconds{ static_cast<int>(osu::od_to_ms300(map->od)) };

	const auto in_time = [time, early_window, late_window](const auto& obj)
		{
			return (obj.time - early_window).count() < time.count()
				&& (obj.time +  late_window).count() > time.count();
		};

	std::copy_if(circles.cbegin(), circles.cend(), std::back_inserter(ret), in_time);
	return ret;
}

std::vector<Slider_object> Data_reader::sliders_at(std::chrono::milliseconds time)
{
	std::vector<Slider_object> ret;

	const auto early_window = std::chrono::milliseconds{ static_cast<int>(osu::ar_to_ms(map->ar)) };
	const auto late_window = std::chrono::milliseconds{ static_cast<int>(osu::od_to_ms300(map->od)) };

	const auto in_time = [time, early_window, late_window](const auto& obj)
		{
			return (obj.time - early_window).count() < time.count()
				&& (obj.time +  late_window).count() > time.count();
		};

	std::copy_if(sliders.cbegin(), sliders.cend(), std::back_inserter(ret), in_time);
	return ret;
}


Magnum::Math::Vector2<std::chrono::milliseconds> Data_reader::time_range()
{
	const auto start = min(sliders.front().time, circles.front().time);	//Todo: Bounds Check
	const auto end = max(sliders.back().time, circles.back().time);
	return { start - 5ms, end + 5ms };
}

void Data_reader::map_window()
{
	if(ImGui::Begin("Load Map")){
		ImGui::InputInt("id", &current_id);
		ImGui::SameLine();
		if(ImGui::Button("load") && current_id > 0) load_map(current_id);
		ImGui::InputTextMultiline("Content", map_string.data(), map_string.size());
	}
	ImGui::End();

	if(ImGui::Begin("Beatmap")){
		if(map){
			const auto label_text = [](const auto label, unsigned long value){
				char value_string[32];
				sprintf(value_string, "%lu", value);
				ImGui::LabelText(label, "%s", value_string);
			};

			ImGui::LabelText("Title", "%s", map->title.c_str());
			ImGui::LabelText("Artist", "%s", map->artist.c_str());
			ImGui::LabelText("Creator", "%s", map->creator.c_str());
			ImGui::LabelText("Difficulty", "%s", map->difficulty_name.c_str());
			ImGui::LabelText("Source", "%s", map->source.c_str());
			label_text("Set ID", map->beatmap_set_id);
			label_text("Map ID", map->beatmap_id);
			ImGui::Text("[Difficulty]");
			ImGui::InputFloat("HP", &map->hp);
			ImGui::InputFloat("CS", &map->cs);
			ImGui::InputFloat("OD", &map->od);
			ImGui::InputFloat("AR", &map->ar);
			ImGui::InputFloat("Slider Multiplier", &map->slider_multiplier);
			ImGui::InputFloat("Slider Tickrate", &map->slider_tick_rate);
			ImGui::Text("[Hitobjects]");
			label_text("Circles", circles.size());
			label_text("Sliders", sliders.size());
			label_text("Spinners", -1);
		}
		else{
			ImGui::Text("Failed loading map");
		}
	}
	ImGui::End();
}

void Data_reader::load_map(const int id)
{
	const auto url = "https://osu.ppy.sh/osu/" + std::to_string(id);
	map_string = get_url(url);

	init_map();
}

void Data_reader::init_map()
{
	map = osu::Beatmap::from_string(map_string);

	if(map && map->mode == Gamemode::osu){
		for (const auto& circle : map->circles){
			circles.emplace_back(Magnum::Vector2{ circle.pos.x, circle.pos.y }, circle.time);
		}

		for (const auto& slider : map->sliders){
			Slider s{};
			for(const auto& segment : slider.points){
				s.emplace_back();
				for(const auto p : segment){
					s.back().emplace_back(p.x, p.y);
				}
			}
			sliders.emplace_back(s, slider.time);
		}
	}
}
