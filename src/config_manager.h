#pragma once

#include <string>

struct Config{
    std::string api_key;
};

class Config_manager{
public:
    Config_manager();
    void load();
    void save();
    void config_window();

    Config config;
    const char* save_status = nullptr;

    Config_manager(const Config_manager&) = delete;
    Config_manager(Config_manager&&) = delete;
    Config_manager& operator=(const Config_manager&) = delete;
    Config_manager& operator=(Config_manager&&) = delete;
};
