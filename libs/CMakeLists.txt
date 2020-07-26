cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(WITH_IMGUI ON CACHE BOOL "" FORCE)
add_subdirectory(magnum-integration EXCLUDE_FROM_ALL)

add_library(imgui_stdlib imgui/misc/cpp/imgui_stdlib.cpp)
target_link_libraries(imgui_stdlib PRIVATE MagnumIntegration::ImGui)
add_library(ImGui::imgui_stdlib ALIAS imgui_stdlib)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

add_subdirectory(httplib)

add_subdirectory(osu_reader)
