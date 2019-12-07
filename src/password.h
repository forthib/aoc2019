#pragma once

#include <vector>

namespace password
{
	bool isValid(size_t number);
	bool isValid(size_t number, std::vector<size_t>& buffer);
}
