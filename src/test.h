#pragma once

#include <exception>

namespace test {
	
	void isTrue(bool value);
	void isFalse(bool value);

	template<typename T, typename U>
	void equals(const T& value, const U& expectedValue)
	{
		if (value != expectedValue)
			throw std::exception{ "value and expected value are different" };
	}
}
