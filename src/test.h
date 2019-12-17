#pragma once

#include <cstdint>
#include <vector>

namespace test {
	
	void isTrue(bool value);
	void isFalse(bool value);
	void equals(std::int64_t value, std::int64_t expectedValue);
	void equals(const std::vector<std::int64_t>& values, const std::vector<std::int64_t>& expectedValues);
	void equals(const char* values, const char* expectedValues);

	template<typename T, typename U>
	void equals(const T& value, const U& expectedValue)
	{
		if (value != expectedValue)
			throw std::exception{ "value and expected value are different" };
	}
}
