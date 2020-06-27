#pragma once

#include <string>
#include <string_view>
#include <vector>


std::vector<std::string_view> split(const std::string_view s, const char delim);
