cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(slider_draw LANGUAGES CXX)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/modules")

find_package(Magnum REQUIRED GL Shaders)
find_package(Corrade REQUIRED Utility)

if (CORRADE_TARGET_EMSCRIPTEN)
    # emscripten ships with a custom implementation of TEST_BIG_ENDIAN in TestBigEndian.cmake that xz requires to compile
    list(INSERT CMAKE_MODULE_PATH 0 "${EMSCRIPTEN_PREFIX}/cmake/Modules")

    # xz comes with methods to check physical memory and cpu cores that fail in our docker container.
    # We can bypass it like this, and everything seems to be working fine
    set(TUKLIB_PHYSMEM_FOUND TRUE)
    set(TUKLIB_CPUCORES_FOUND TRUE)

    find_package(Magnum REQUIRED Sdl2Application)   # Todo: Move this to emscripten
    # find_package(Magnum REQUIRED EmscriptenApplication)
else ()
    find_package(Magnum REQUIRED Sdl2Application)

    # WebGL doesn't need OpenSSL because of javascript
    find_package(OpenSSL REQUIRED)
endif ()

add_subdirectory(libs)

# shaders
corrade_add_resource(ShaderResources res/shaders.conf)
add_library(shaders_resources OBJECT ${ShaderResources})
target_link_libraries(shaders_resources Corrade::Utility)

# data
corrade_add_resource(DataResources res/data.conf)
add_library(data_resources OBJECT ${DataResources})
target_link_libraries(data_resources Corrade::Utility)


set(SOURCES
        src/main.cpp
        src/string_helpers.cpp
        src/data_reader.cpp
        src/play_container.cpp
        src/render/vertex_generate.cpp
        src/api_manager.cpp
        src/render/shaders/sliderbody_shader.cpp
        src/render/shaders/circleobject_shader.cpp
        src/render/shaders/minimal_shader.cpp
        src/render/circleobject_renderer.cpp
        src/render/coordinate_converter.cpp
        src/render/slider_renderer.cpp
        src/render/line_renderer.cpp
        src/config_manager.cpp
        src/replay_container.cpp
        src/notification_manager.cpp
        )
add_executable(slider_draw
        ${SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/version.cpp

        $<TARGET_OBJECTS:shaders_resources>
        $<TARGET_OBJECTS:data_resources>)

# allow inclusion of headers from other folders (version.cpp)
target_include_directories(slider_draw PRIVATE src)

set_target_properties(slider_draw PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CORRADE_USE_PEDANTIC_FLAGS ON
        )

if (CORRADE_TARGET_EMSCRIPTEN)
    set_target_properties(slider_draw PROPERTIES
            LINK_FLAGS "-lidbfs.js -s EXPORTED_FUNCTIONS='[_main, _loaded_callback]' -s EXPORTED_RUNTIME_METHODS='[ccall]' -s ASYNCIFY=1 -s ASYNCIFY_IMPORTS='[js_api_request]' -s ASYNCIFY_STACK_SIZE=65535"
            )
    # -lidbfs.js for dbfs file system
    # EXPORTED_FUNCTIONS to allow access from js
    # EXPORTED_RUNTIME_METHODS to allow access from js
    # ASYNCIFY to allow async js calls
    # ASYNCIFY_STACK_SIZE to avoid "RuntimeError: unreachable executed" in async code
endif ()

target_link_libraries(slider_draw PRIVATE
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders
        MagnumIntegration::ImGui
        ImGui::imgui_stdlib
        Corrade::Utility
        nlohmann_json::nlohmann_json
        osuReader::osuReader
        )

# WebGL doesn't need OpenSSL because of javascript
if (NOT CORRADE_TARGET_EMSCRIPTEN)
    target_link_libraries(slider_draw PRIVATE
            OpenSSL::SSL
            OpenSSL::Crypto
            httplib::httplib
            )
endif ()

# Include Build Information

# get git commit hash
execute_process(COMMAND
        "${GIT_EXECUTABLE}" describe --always --abbrev=40 --dirty
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
        )

# get git commit time
execute_process(COMMAND
        "${GIT_EXECUTABLE}" log -1 --format=%ad --date=iso
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_DATE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
        )

# get build time
string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M:%S")

# apply variables
configure_file(src/version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY)


# For Emscripten copy the boilerplate next to the executable so it can be run
# directly from the build dir; provide an install target as well
if (CORRADE_TARGET_EMSCRIPTEN)
    add_custom_command(TARGET slider_draw POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
            ${MAGNUM_WEBAPPLICATION_CSS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/index.html
            $<TARGET_FILE_DIR:slider_draw>)

    install(TARGETS slider_draw DESTINATION ${MAGNUM_DEPLOY_PREFIX})
    install(FILES
            index.html
            ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
            ${MAGNUM_WEBAPPLICATION_CSS}
            DESTINATION ${MAGNUM_DEPLOY_PREFIX})
    install(FILES
            $<TARGET_FILE_DIR:slider_draw>/slider_draw.js.mem
            $<TARGET_FILE_DIR:slider_draw>/slider_draw.wasm
            DESTINATION ${MAGNUM_DEPLOY_PREFIX} OPTIONAL)
endif ()

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT slider_draw)

set(CMAKE_CXX_CLANG_TIDY clang-tidy)
add_custom_target(
        clang-tidy-check clang-tidy --config-file=.clang-tidy -p ${CMAKE_BINARY_DIR}/compile_commands.json -header-filter=${CMAKE_SOURCE_DIR}/src/.* ${SOURCES}
        DEPENDS ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(
        clang-format-check clang-format -i --style=file ${SOURCES} ./src/*.h
        DEPENDS ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
