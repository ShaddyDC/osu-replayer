#include "string_helpers.h"
#include <algorithm>

std::vector<std::string_view> split(const std::string_view s, const char delim)
{
	std::vector<std::string_view> ret;
	auto it       = std::find(s.begin(), s.end(), delim);
	auto it_start = s.begin();

	const auto add_if_not_empty = [&]
	{
		if(it != it_start){
			ret.emplace_back(s.data() + (it_start - s.begin()), it - it_start);
		}
	};

	for(; it != s.end(); it = std::find(it, s.end(), delim)){
        add_if_not_empty();
        it_start = it = it + 1;
	}
	add_if_not_empty();
	return ret;
}