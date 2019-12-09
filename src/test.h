#pragma once

#include <cstdint>
#include <vector>

namespace test {
	void isTrue(bool value);
	void isFalse(bool value);
	void equals(std::int64_t value, std::int64_t expectedValue);
	void equals(const std::vector<std::int64_t>& values, const std::vector<std::int64_t>& expectedValues);
}
