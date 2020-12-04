//
// Created by space on 23.10.20.
//

#ifndef SLIDER_DRAW_API_MANAGER_H
#define SLIDER_DRAW_API_MANAGER_H

#include <string>
#include <string_view>

class Api_manager {
public:
    explicit Api_manager(const std::string& api_key);

    std::string beatmap(std::string_view id);

    Api_manager(const Api_manager&) = delete;
    Api_manager(Api_manager&&) = delete;
    Api_manager& operator=(const Api_manager&) = delete;
    Api_manager& operator=(Api_manager&&) = delete;

private:
    std::string api_request(std::string_view endpoint);

    static constexpr const inline auto api_base_url = "https://osutest.shaddy.dev";
    //    static constexpr const inline auto api_base_url = "http://localhost";
    const std::string& api_key;
};


#endif//SLIDER_DRAW_API_MANAGER_H
