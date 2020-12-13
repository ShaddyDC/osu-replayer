//
// Created by space on 23.10.20.
//

#include "api_manager.h"
#include <Corrade/Utility/Debug.h>
#include <Magnum/Magnum.h>

Api_manager::Api_manager(const std::string& api_key) : api_key{api_key}
{
}

std::string Api_manager::beatmap(std::string_view id)
{
    const auto endpoint = "/api/v1/osufile/" + std::string{id};
    return api_request(endpoint);
}

#if defined(MAGNUM_TARGET_WEBGL)
#include <emscripten.h>

EM_JS(char*, js_api_request, (const char* url, const char* api_key), {
    return Asyncify.handleAsync(async function() {
        const response = await fetch(UTF8ToString(url), {
            headers: {
                "api-key": UTF8ToString(api_key)
            }
        });
        const text = await response.text();
        return allocate(intArrayFromString(text, false), 'i8', ALLOC_NORMAL);
    });
});

std::string Api_manager::api_request(std::string_view endpoint)
{
    const auto url = std::string{api_base_url} + std::string{endpoint};

    auto* buffer = js_api_request(url.c_str(), api_key.c_str());
    Corrade::Utility::Debug() << "buffer " << (int) buffer;

    if(buffer) {
        std::string response(buffer);
        free(buffer);
        return response;
    }
    return "Failed :(";
}

#else

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

std::string Api_manager::api_request(std::string_view endpoint)
{
    Corrade::Utility::Debug() << "Trying to load " << api_base_url << endpoint.data();

    httplib::Client client{api_base_url};
    httplib::Headers headers{
            {"api-key", api_key.c_str()}};

    const auto response = client.Get(endpoint.data(), headers);
    Corrade::Utility::Debug() << "Error " << response.error();

    if(response) return response->body;
    return "Failed :(";
}

#endif