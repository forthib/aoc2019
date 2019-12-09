#pragma once

#include <vector>

namespace image {
	void check(size_t width, size_t height, const std::vector<std::int64_t>& data);
	void displayRaw(size_t width, size_t height, const std::vector<std::int64_t>& data);
	void displayDecoded(size_t width, size_t height, const std::vector<std::int64_t>& data, bool pretty = false);
}
