#include "api_manager.h"
#include "Magnum/Magnum.h"// Needed for MAGNUM_TARGET_WEBGL
#include "notification_manager.h"
#include <Corrade/Utility/Debug.h>

Api_manager::Api_manager(const std::string& api_key) : api_key{api_key}
{
}

std::optional<std::string> Api_manager::beatmap(std::string_view id)
{
    const auto endpoint = "/api/v1/osufile/" + std::string{id};
    return api_request(endpoint);
}

#if defined(MAGNUM_TARGET_WEBGL)
#include <emscripten.h>

EM_JS(void, js_api_request, (const char* url, const char* api_key, char** body, int* status), {
    return Asyncify.handleAsync(async function() {
        const response = await fetch(UTF8ToString(url), {
            headers: {
                "api-key": UTF8ToString(api_key)
            }
        });
        const text = await response.text();
        const buffer = allocate(intArrayFromString(text, false), 'i8', ALLOC_NORMAL);

        setValue(body, buffer, 'i32');
        setValue(status, response.status, 'i32');
    });
});

std::optional<std::string> Api_manager::api_request_impl(std::string_view endpoint, int& status)
{
    const auto url = std::string{api_base_url} + std::string{endpoint};

    char* text = nullptr;
    js_api_request(url.c_str(), api_key.c_str(), &text, &status);

    Corrade::Utility::Debug() << "web req result" << status << text;

    if(status == 200) {
        std::string s{text};
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
    Corrade::Utility::Debug() << "Trying to load " << api_base_url << endpoint.data();

    httplib::Client client{api_base_url};
    httplib::Headers headers{
            {"api-key", api_key.c_str()}};

    const auto response = client.Get(endpoint.data(), headers);
    Corrade::Utility::Debug() << "Error " << response.error();

    if(response) {
        status = response->status;
        if(status != 200) return std::nullopt;
        return response->body;
    }
    Corrade::Utility::Debug() << "Bad error, no response";
    return std::nullopt;
}

#endif

std::optional<std::string> Api_manager::api_request(std::string_view endpoint)
{
    int status = 0;
    auto res = api_request_impl(endpoint, status);

    if(status == 401) {
        Corrade::Utility::Debug() << "Resource needs Authentication";
        Notification_manager{}.add_notification("auth_req",
                                                "You need to be authenticated to get this resource.\n"
                                                "This is because it is not yet stored on the server and requires a user token to access it.\n");
    }

    return res;
}
