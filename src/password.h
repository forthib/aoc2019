#pragma once

#include <vector>

namespace password
{
	bool isValid(size_t number, bool strictAdjacency);
	bool isValid(size_t number, bool strictAdjacency, std::vector<size_t>& buffer);
}
