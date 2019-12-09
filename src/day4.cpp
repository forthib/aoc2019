#include "password.h"
#include "test.h"

#include <iostream>

namespace {
	size_t countValidPasswords(size_t from, size_t to, bool strictAdjacency)
	{
		auto   buffer = std::vector<size_t>{};
		size_t cpt    = 0;
		for (size_t number = from; number <= to; ++number)
			if (password::isValid(number, strictAdjacency, buffer))
				++cpt;
		return cpt;
	}
}

int main(int argc, char* argv[])
{
	test::isTrue(password::isValid(111111, false));
	test::isFalse(password::isValid(223450, false));
	test::isFalse(password::isValid(123789, false));

	test::isTrue(password::isValid(112233, true));
	test::isFalse(password::isValid(123444, true));
	test::isTrue(password::isValid(111122, true));

	const size_t from = 130254;
	const size_t to   = 678275;
	std::cout << "Part1: " << countValidPasswords(from, to, false) << "\n";
	std::cout << "Part2: " << countValidPasswords(from, to, true) << "\n";

	std::cin.get();
}
