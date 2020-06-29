cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(slider_draw LANGUAGES CXX)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/modules")

find_package(Magnum REQUIRED GL Shaders)
find_package(Corrade REQUIRED Utility)

if(CORRADE_TARGET_EMSCRIPTEN)
    find_package(Magnum REQUIRED EmscriptenApplication)
else()
    find_package(Magnum REQUIRED Sdl2Application)
endif()

add_subdirectory(libs)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

# shaders
corrade_add_resource(ShaderResources res/shaders.conf)
add_library(shaders_resources OBJECT ${ShaderResources})
target_link_libraries(shaders_resources Corrade::Utility)

# data
corrade_add_resource(DataResources res/data.conf)
add_library(data_resources OBJECT ${DataResources})
target_link_libraries(data_resources Corrade::Utility)

add_executable(slider_draw
	src/main.cpp
	src/string_helpers.cpp
	src/slider.cpp
	src/data_reader.cpp
	src/play_container.cpp
    src/render/vertex_generate.cpp
    src/render/shaders/sliderbody_shader.cpp
    src/render/shaders/circleobject_shader.cpp
    src/render/shaders/minimal_shader.cpp
    src/render/circleobject_renderer.cpp
    src/render/coordinate_converter.cpp
    src/render/slider_renderer.cpp
    src/render/line_renderer.cpp
    
	$<TARGET_OBJECTS:shaders_resources>
	$<TARGET_OBJECTS:data_resources>
)

set_target_properties(slider_draw PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_options(slider_draw PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

target_link_libraries(slider_draw PRIVATE
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::Shaders
    MagnumIntegration::ImGui
	Corrade::Utility
)

# For Emscripten copy the boilerplate next to the executable so it can be run
# directly from the build dir; provide an install target as well
if(CORRADE_TARGET_EMSCRIPTEN)
    add_custom_command(TARGET slider_draw POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
            ${MAGNUM_WEBAPPLICATION_CSS}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/slider_draw.html
            $<TARGET_FILE_DIR:slider_draw>)

    install(TARGETS slider_draw DESTINATION ${MAGNUM_DEPLOY_PREFIX})
    install(FILES
    slider_draw.html
        ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
        ${MAGNUM_WEBAPPLICATION_CSS}
        DESTINATION ${MAGNUM_DEPLOY_PREFIX})
    install(FILES
        $<TARGET_FILE_DIR:slider_draw>/slider_draw.js.mem
        $<TARGET_FILE_DIR:slider_draw>/slider_draw.wasm
        DESTINATION ${MAGNUM_DEPLOY_PREFIX} OPTIONAL)
endif()

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT slider_draw)
