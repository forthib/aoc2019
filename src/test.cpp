#include "test.h"

#include <string>

namespace test {
	void isTrue(bool value)
	{
		if (!value)
			throw std::exception{ "value is not true" };
	}

	void isFalse(bool value)
	{
		if (value)
			throw std::exception{ "value is not false" };
	}

	void equals(std::int64_t value, std::int64_t expectedValue)
	{
		if (value != expectedValue)
			throw std::exception{ "value and expected value are different" };
	}

	void equals(const std::vector<std::int64_t>& values, const std::vector<std::int64_t>& expectedValues)
	{
		if (values != expectedValues)
			throw std::exception{ "values and expected values are different" };
	}

	void equals(const char* values, const char* expectedValues)
	{ //
		equals(std::string{ values }, std::string{ expectedValues });
	}
}