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

    void update_api_key(std::string api_key);

    Config config;
    const char* save_status = nullptr;

    Config_manager(const Config_manager&) = delete;
    Config_manager(Config_manager&&) = delete;
    Config_manager& operator=(const Config_manager&) = delete;
    Config_manager& operator=(Config_manager&&) = delete;

private:
    std::string cli_updated_api_key;
};
