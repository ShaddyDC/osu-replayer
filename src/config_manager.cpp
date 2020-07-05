#include "config_manager.h"

#include <nlohmann/json.hpp>
#include <array>
#include <fstream>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <filesystem>
#include <stdlib.h>

// Platform dependent code
#if defined(_WIN32) || defined(WIN32) 
#include <Shlobj.h>
#include <Shlobj_core.h>
// Untested
wchar_t appdata[MAX_PATH];
const auto appdata = SHGetSpecialFolderPath(hWnd, buffer, CSIDL_LOCAL_APPDATA, false);
std::filesystem::path folder{ buffer };
if(!result){
    result = folder = "./";
}
const auto config_file = foler / "replay_viewer.conf";

const auto config_file = std::filesystem::path{ "/data" } / ".replay_viewer.conf";

#elif defined(MAGNUM_TARGET_WEBGL)
const auto config_file = std::filesystem::path{ "/data" } / ".replay_viewer.conf";

#include <emscripten.h>

// Probably shouldn't do this
// Todo: Improve
static Config_manager* instance;
extern "C" {
    void loaded_callback(void); // Prototype to prevent warning
    void loaded_callback(void)
    {
        instance->load();
        ImGui::GetIO().IniFilename = "/data/imgui.ini";
        ImGui::LoadIniSettingsFromDisk("/data/imgui.ini");
    }
}

#elif defined(__unix__)
const auto config_file = std::filesystem::path{ getenv("HOME") } / ".replay_viewer.conf";
#else
// Unknown platform
const auto config_file = std::filesystem::path{ ".replay_viewer.conf" };
#endif

Config_manager::Config_manager()
{
    // Mount filesystem for webasm
#ifdef MAGNUM_TARGET_WEBGL
    instance = this;
    // EM_ASM is a macro to call in-line JavaScript code.
    EM_ASM(
        console.log("Mounting IDBFS file system");
        // Make a directory other than '/'
        FS.mkdir('/data');
        // Then mount with IDBFS type
        FS.mount(IDBFS, { root: '.' }, '/data');

        // Then sync
        FS.syncfs(true, function (err) {
            assert(!err);
            console.log("Finished loading files system!");
            Module.ccall('loaded_callback');
        });
    );
#else
    load();
#endif
}

void Config_manager::load()
{
    nlohmann::json json{};

    std::ifstream file{ config_file };
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
        const auto error = strerror(errno);
        Magnum::Debug() << "Couldn't write to config file \"" << config_file.c_str() << "\": " << error;
        save_status = error;
    }

    file << json;

    // Apply changes on emscripten
    #if defined(MAGNUM_TARGET_WEBGL)
    EM_ASM(
        console.log("Syncing write");
        FS.syncfs(false, function (err) {
            assert(!err);
        });
    );
    #endif

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
        ImGui::SameLine();
        if(ImGui::Button("Save")) save();
        if(save_status) {
            ImGui::Text("%s", save_status);
        }
    }
    ImGui::End();
}
