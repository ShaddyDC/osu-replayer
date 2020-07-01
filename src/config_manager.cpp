#include "config_manager.h"

#include <nlohmann/json.hpp>
#include <array>
#include <fstream>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <filesystem>
#include <stdlib.h>

#ifdef MAGNUM_TARGET_WEBGL
static_assert(false, "Todo");
#elif defined(_WIN32) || defined(WIN32) 
static_assert(false, "Todo");
#elif __unix__
const auto config_file = std::filesystem::path{ getenv("HOME") } / ".replay_viewer.conf";
#else
static_assert(false, "Unknown platform");
#endif

Config_manager::Config_manager()
{
    load();
}

void Config_manager::load()
{
    std::ifstream file{ config_file };
    nlohmann::json json{};
    if(file){
        try{
            file >> json;
        }catch(const nlohmann::detail::parse_error& e){
            Magnum::Debug() << "Couldn't parse config file." << config_file.c_str() << e.what();
            json = nlohmann::json::object();
        }
    }
    else {
        Magnum::Debug() << "Couldn't read config file." << config_file.c_str() << strerror(errno);
    }

    // Make default loading work
    if(json.type() != nlohmann::json::value_t::object)
        json = nlohmann::json::object();

    config.api_key = json.value("api_key", "");
}

void Config_manager::save()
{
    nlohmann::json json;
    json["api_key"] = config.api_key;

    std::ofstream file{ config_file };
    if(!file){
        save_status = strerror(errno);
        Magnum::Debug() << "Couldn't write to config file \"" << config_file.c_str() << "\": " << save_status;
        return;
    }
    else save_status = nullptr;
    file << json;
    if(file){
        save_status = "Saved successfully";
    } else{
        save_status = "Something may or may not have gone wrong. Good luck!";
    }
}

void Config_manager::config_window()
{
    if(ImGui::Begin("config")){
        ImGui::InputText("Api Key", &config.api_key);
        if(ImGui::Button("Reload")) load();
        if(ImGui::Button("Save")) save();
        if(save_status) {
            ImGui::Text(save_status);
        }
        ImGui::End();
    }
}
