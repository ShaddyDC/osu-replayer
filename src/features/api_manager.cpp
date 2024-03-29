#include "api_manager.h"
#include "Corrade/Utility/Debug.h"
#include "Magnum/Magnum.h"// Needed for MAGNUM_TARGET_WEBGL
#include "nlohmann/json.hpp"
#include "notification_manager.h"

static constexpr const auto status_success = 200;
static constexpr const auto status_forbidden = 401;

Api_manager::Api_manager(const std::string& api_key, Notification_manager& notification_manager)
    : api_key{api_key}, notification_manager{notification_manager}
{
}

std::optional<std::string> Api_manager::beatmap(std::string_view id)
{
    const auto endpoint = "/api/v1/osufile/" + std::string{id};
    return api_request(endpoint);
}

#if defined(MAGNUM_TARGET_WEBGL)
#include <emscripten.h>

EM_JS(void, js_api_request, (const char* url, const char* api_key, char** body, int* strlen, int* status), {
    return Asyncify.handleAsync(async function() {
        const response = await fetch(UTF8ToString(url), {
            headers: {
                "api-key": UTF8ToString(api_key)
            }
        });
        const bytes = new Uint8Array(await response.arrayBuffer());
        const buffer = Module._malloc(bytes.byteLength);
        writeArrayToMemory(bytes, buffer);

        setValue(body, buffer, 'i32');
        setValue(strlen, bytes.byteLength, 'i32');
        setValue(status, response.status, 'i32');
    });
});

std::optional<std::string> Api_manager::api_request_impl(std::string_view endpoint, int& status)
{
    const auto url = std::string{api_base_url} + std::string{endpoint};

    char* text = nullptr;
    int strlen = 0;
    js_api_request(url.c_str(), api_key.c_str(), &text, &strlen, &status);

    Corrade::Utility::Debug() << "web req result" << status << text << strlen;

    if(status == status_success) {
        std::string s{text, static_cast<std::size_t>(strlen)};
        delete text;
        return s;
    }
    if(text) delete text;
    return std::nullopt;
}

#else

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

std::optional<std::string> Api_manager::api_request_impl(std::string_view endpoint, int& status)
{
    httplib::Client client{api_base_url};
    httplib::Headers headers{
            {"api-key", api_key.c_str()}};

    const auto response = client.Get(endpoint.data(), headers);

    if(response) {
        status = response->status;
        if(status != status_success) {
            Corrade::Utility::Debug() << "Error " << static_cast<int>(response.error()) << response->status << response->body.c_str();
            return std::nullopt;
        }
        return response->body;
    }
    Corrade::Utility::Debug() << "Bad error, no response" << static_cast<int>(response.error());
    return std::nullopt;
}

#endif

std::optional<std::string> Api_manager::api_request(std::string_view endpoint)
{
    Corrade::Utility::Debug() << "Trying to load " << api_base_url << endpoint.data();

    int status = 0;
    auto res = api_request_impl(endpoint, status);

    if(status == status_forbidden) {
        Corrade::Utility::Debug() << "Resource needs Authentication";
        notification_manager.add_notification("auth_req",
                                              "You need to be authenticated to get this resource.\n"
                                              "This is because it is not yet stored on the server and requires a user token to access it.\n"
                                              "Head to https://osuapi.shaddy.dev/ to authenticate, follow the instructions, and choose \"Load in Application\"\n");
    }

    return res;
}

std::optional<int> Api_manager::beatmap_id(std::string_view hash)
{
    const auto endpoint = "/api/v1/beatmaps/lookup/s/" + std::string{hash};
    const auto res = api_request(endpoint);
    if(!res) return std::nullopt;

    auto json = nlohmann::json::parse(*res, nullptr, false);
    if(json.is_discarded()) {
        Corrade::Utility::Debug() << "Invalid json" << res->c_str();
        return std::nullopt;
    }
    if(!json["id"].is_number_integer()) {
        Corrade::Utility::Debug() << "id is not integer"
                                  << res->c_str();
        return std::nullopt;
    }
    return json["id"].get<int>();
}

std::optional<std::string> Api_manager::replay(std::string_view id)
{
    const auto endpoint = "/api/v1/scorefile/osu/" + std::string{id};
    return api_request(endpoint);
}
