#include "password.h"

#include <algorithm>

namespace password
{
	namespace
	{
		std::vector<size_t> getDigits(size_t number, std::vector<size_t> buffer = {})
		{
			buffer.clear();
			while (number >= 10)
			{
				buffer.push_back(number % 10);
				number /= 10;
			}
			buffer.push_back(number);

			std::reverse(buffer.begin(), buffer.end());
			return buffer;
		}

		bool hasSameAdjacentDigits(const std::vector<size_t>& digits)
		{
			for (size_t i = 1; i < digits.size(); ++i)
				if (digits[i - 1] == digits[i] && (i < 2 || digits[i - 2] != digits[i - 1]) && (i > digits.size() - 2 || digits[i + 1] != digits[i]))
					return true;
			return false;
		}
	}

	bool isValid(size_t number)
	{
		auto buffer = std::vector<size_t>{};
		return isValid(number, buffer);
	}

	bool isValid(size_t number, std::vector<size_t>& buffer)
	{
		buffer = getDigits(number, std::move(buffer));

		if (buffer.size() != 6)
			return false;

		if (!hasSameAdjacentDigits(buffer))
			return false;

		if (!std::is_sorted(buffer.begin(), buffer.end()))
			return false;
		
		return true;
	}
}
