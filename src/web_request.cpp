#include "web_request.h"

#include <Magnum/Magnum.h>
#include <Corrade/Utility/Debug.h>

#if defined(MAGNUM_TARGET_WEBGL)
#include <emscripten.h>

std::string get_url(const std::string& url)
{
    // proxy to allow cross-origin resource sharing
    const auto cors_url = "https://cors-anywhere.herokuapp.com/" + url;
#if 0   // Broken for some reason
    // Also requires "-s ASYNCIFY=1" if enabled
    char* buffer = nullptr;
    int size, error;
    emscripten_wget_data(
        url.c_str(),
        (void**)&buffer,
        &size,
        &error
    );
    if(size > 0){
        std::string response((char*)buffer, size);
        free(buffer);
        return response;
    }
    return "";
#else
    char* buffer = (char*)EM_ASM_ARGS({
        let url = UTF8ToString($0);
        console.log(`url: ${url}`);
        let request = new XMLHttpRequest();
        request.open("GET", url, false); // false: synchrone, true: async
        request.send(null);
        if(request.status === 200) {
            let response = request.responseText;
            console.log("Response: %s", response);
            return allocate(intArrayFromString(response, false), 'i8', ALLOC_NORMAL);
        } 
        else{
            console.log("Failed :(", request.status, request.statusText);
            return 0;
        }
    }, cors_url.c_str());
    if(buffer){
        std::string response(buffer);
        free(buffer);
        return response;
    }
    return "";
#endif
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
