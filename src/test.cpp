#include "test.h"

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
}