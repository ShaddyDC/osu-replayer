cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

message("${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(WITH_IMGUI ON CACHE BOOL "" FORCE)
add_subdirectory(magnum-integration EXCLUDE_FROM_ALL)