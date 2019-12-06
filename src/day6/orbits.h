#pragma once

#include <array>
#include <string>
#include <vector>

namespace orbits
{
	size_t count(const std::vector<std::array<std::string, 2>>& map);
	size_t shortest(const std::vector<std::array<std::string, 2>>& map, const std::string& from, const std::string& to);
}