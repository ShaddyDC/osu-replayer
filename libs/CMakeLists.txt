cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(MAGNUM_WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)
set(MAGNUM_GL_INFO ON CACHE BOOL "" FORCE)

add_subdirectory(corrade EXCLUDE_FROM_ALL)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

if (CORRADE_TARGET_EMSCRIPTEN)
    # emscripten ships with a custom implementation of TEST_BIG_ENDIAN in TestBigEndian.cmake that xz requires to compile
    list(INSERT CMAKE_MODULE_PATH 0 "${EMSCRIPTEN_PREFIX}/cmake/Modules")

    # xz comes with methods to check physical memory and cpu cores that fail in our docker container.
    # We can bypass it like this, and everything seems to be working fine
    set(TUKLIB_PHYSMEM_FOUND TRUE)
    set(TUKLIB_CPUCORES_FOUND TRUE)
endif ()

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(MAGNUM_WITH_IMGUI ON CACHE BOOL "" FORCE)
add_subdirectory(magnum-integration EXCLUDE_FROM_ALL)

add_library(imgui_stdlib imgui/misc/cpp/imgui_stdlib.cpp)
target_link_libraries(imgui_stdlib PRIVATE MagnumIntegration::ImGui)
add_library(ImGui::imgui_stdlib ALIAS imgui_stdlib)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

add_subdirectory(httplib)

set(ENABLE_TESTING OFF CACHE INTERNAL "")
set(ENABLE_LZMA ON CACHE INTERNAL "")
add_subdirectory(osu_reader)
