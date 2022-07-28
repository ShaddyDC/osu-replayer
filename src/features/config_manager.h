#pragma once

#include "util/component.h"
#include <string>

struct Config {
    std::string api_key;
};

class Config_manager : public Component {
public:
    Config_manager();
    void load();
    void save();

    void draw() override;

    void update_api_key(std::string api_key);
    [[nodiscard]] bool is_loaded();

    Config config;
    const char* save_status = nullptr;

    Config_manager(const Config_manager&) = delete;
    Config_manager(Config_manager&&) = delete;
    Config_manager& operator=(const Config_manager&) = delete;
    Config_manager& operator=(Config_manager&&) = delete;
    ~Config_manager() override = default;

private:
    void config_window();

    std::string cli_updated_api_key;
};
