#ifndef SLIDER_DRAW_API_MANAGER_H
#define SLIDER_DRAW_API_MANAGER_H

#include "notification_manager.h"
#include <optional>
#include <string>
#include <string_view>

class Api_manager {
public:
    explicit Api_manager(const std::string& api_key, Notification_manager& notification_manager);

    [[nodiscard]] std::optional<std::string> beatmap(std::string_view id);
    [[nodiscard]] std::optional<std::string> replay(std::string_view id);
    [[nodiscard]] std::optional<int> beatmap_id(std::string_view hash);

    Api_manager(const Api_manager&) = delete;
    Api_manager(Api_manager&&) = delete;
    Api_manager& operator=(const Api_manager&) = delete;
    Api_manager& operator=(Api_manager&&) = delete;
    ~Api_manager() = default;

private:
    std::optional<std::string> api_request(std::string_view endpoint);
    std::optional<std::string> api_request_impl(std::string_view endpoint, int& status);

    static constexpr const inline auto api_base_url = "https://osuapi.shaddy.dev";
    //    static constexpr const inline auto api_base_url = "https://localhost";
    const std::string& api_key;
    Notification_manager& notification_manager;
};


#endif//SLIDER_DRAW_API_MANAGER_H
