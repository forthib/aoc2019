#pragma once

#include <vector>

namespace image
{
	void check(size_t width, size_t height, const std::vector<int>& data);
	void displayRaw(size_t width, size_t height, const std::vector<int>& data);
	void displayDecoded(size_t width, size_t height, const std::vector<int>& data, bool pretty = false);
}
