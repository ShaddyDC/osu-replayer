cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

add_library(httplib INTERFACE)
target_include_directories(httplib SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(httplib::httplib ALIAS httplib)
