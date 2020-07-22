#include "web_request.h"

#include <Magnum/Magnum.h>
#include <Corrade/Utility/Debug.h>

// TODO Make webrequests a promise or something

#if defined(MAGNUM_TARGET_WEBGL)
#include <emscripten.h>

std::string get_url(const std::string& url)
{
    // proxy to allow cross-origin resource sharing
    const auto cors_url = "https://cors-anywhere.herokuapp.com/" + url;
    // Also requires "-s ASYNCIFY=1" if enabled
    char* buffer = nullptr;
    int size, error;
    emscripten_wget_data(
        cors_url.c_str(),
        reinterpret_cast<void**>(&buffer),
        &size,
        &error
    );
    if(size > 0){
        std::string response(buffer, size);
        free(buffer);
        return response;
    }
    return "Failed :(";
}

#else

#include <string_view>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

std::string get_url(const std::string& url)
{
    std::string_view url_view = url;

    url_view.remove_prefix(std::min(url_view.find("//") + 2, url_view.size()));
    auto host = url_view;
    host.remove_suffix(url_view.size() - std::min(url_view.find("/"), url_view.size()));
    auto path = url_view;
    path.remove_prefix(host.size());

    const auto host_string = std::string{ host };
    const auto path_string = std::string{ path };

    httplib::SSLClient client(host_string);

	const auto response = client.Get(path_string.c_str());
    if(response && response->status == 200){
        return response->body;
    }

    return "Failed :(";  //Todo: Better error handling
}

#endif
