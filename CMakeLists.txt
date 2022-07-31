cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(osu_replayer LANGUAGES CXX)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/modules")

add_subdirectory(libs)

find_package(Magnum REQUIRED GL Shaders)
find_package(Corrade REQUIRED Utility)

if (CORRADE_TARGET_EMSCRIPTEN)
    find_package(Magnum REQUIRED Sdl2Application)   # Todo: Move this to emscripten
    # find_package(Magnum REQUIRED EmscriptenApplication)
else ()
    find_package(Magnum REQUIRED Sdl2Application)

    # WebGL doesn't need OpenSSL because of javascript
    find_package(OpenSSL REQUIRED)
endif ()

# shaders
corrade_add_resource(ShaderResources res/shaders.conf)
add_library(shaders_resources OBJECT ${ShaderResources})
target_link_libraries(shaders_resources Corrade::Utility)

# data
corrade_add_resource(DataResources res/data.conf)
add_library(data_resources OBJECT ${DataResources})
target_link_libraries(data_resources Corrade::Utility)

# Program as library
set(SOURCES
        src/util/string_helpers.cpp
        src/data/beatmap_reader.cpp
        src/data/play_container.cpp
        src/render/vertex_generate.cpp
        src/features/api_manager.cpp
        src/render/shaders/sliderbody_shader.cpp
        src/render/shaders/circleobject_shader.cpp
        src/render/shaders/minimal_shader.cpp
        src/render/circleobject_renderer.cpp
        src/render/coordinate_converter.cpp
        src/render/slider_renderer.cpp
        src/render/line_renderer.cpp
        src/features/config_manager.cpp
        src/data/replay_loader.cpp
        src/features/notification_manager.cpp
        src/playfield/border.cpp
        src/playfield/playback_logic.cpp
        src/playfield/playfield_size_manager.cpp
        src/playfield/playfield_coordinate_provider.cpp
        src/playfield/visible_hitobjects_manager.cpp
        src/data/analysed_beatmap.cpp
        src/data/analysed_replay.cpp
        src/data/replay_beatmap_matcher.cpp
        src/playfield/mouse_provider.cpp
        )

add_library(osu_replayer_lib
        ${SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

target_include_directories(osu_replayer_lib PUBLIC src)

set_target_properties(osu_replayer_lib PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CORRADE_USE_PEDANTIC_FLAGS ON
        )

target_link_libraries(osu_replayer_lib PUBLIC
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders
        MagnumIntegration::ImGui
        ImGui::imgui_stdlib
        Corrade::Utility
        nlohmann_json::nlohmann_json
        osuReader::osuReader
        range-v3::range-v3
        )

# WebGL doesn't need OpenSSL because of javascript
if (NOT CORRADE_TARGET_EMSCRIPTEN)
    target_link_libraries(osu_replayer_lib PRIVATE
            OpenSSL::SSL
            OpenSSL::Crypto
            httplib::httplib
            )
endif ()

# Main executable
add_executable(osu_replayer src/main.cpp
        $<TARGET_OBJECTS:shaders_resources>
        $<TARGET_OBJECTS:data_resources>)

set_target_properties(osu_replayer PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CORRADE_USE_PEDANTIC_FLAGS ON
        )

target_link_libraries(osu_replayer PRIVATE osu_replayer_lib)

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

if (CORRADE_TARGET_EMSCRIPTEN)
    set_target_properties(osu_replayer PROPERTIES
            LINK_FLAGS "-lidbfs.js -s EXPORTED_FUNCTIONS='[_main, _loaded_callback]' -s EXPORTED_RUNTIME_METHODS='[ccall]' -s ASYNCIFY=1 -s ASYNCIFY_IMPORTS='[js_api_request]' -s ASYNCIFY_STACK_SIZE=65535"
            )
    # -lidbfs.js for dbfs file system
    # EXPORTED_FUNCTIONS to allow access from js
    # EXPORTED_RUNTIME_METHODS to allow access from js
    # ASYNCIFY to allow async js calls
    # ASYNCIFY_STACK_SIZE to avoid "RuntimeError: unreachable executed" in async code

    # For Emscripten copy the boilerplate next to the executable so it can be run
    # directly from the build dir; provide an install target as well
    add_custom_command(TARGET osu_replayer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
            ${MAGNUM_WEBAPPLICATION_CSS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/index.html
            $<TARGET_FILE_DIR:osu_replayer>)

    install(TARGETS osu_replayer DESTINATION ${MAGNUM_DEPLOY_PREFIX})
    install(FILES
            index.html
            ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
            ${MAGNUM_WEBAPPLICATION_CSS}
            DESTINATION ${MAGNUM_DEPLOY_PREFIX})
    install(FILES
            $<TARGET_FILE_DIR:osu_replayer>/osu_replayer.js.mem
            $<TARGET_FILE_DIR:osu_replayer>/osu_replayer.wasm
            DESTINATION ${MAGNUM_DEPLOY_PREFIX} OPTIONAL)
endif ()

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT osu_replayer)

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
