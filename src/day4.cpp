#include "password.h"

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << std::boolalpha;
	std::cout << "111111: " << password::isValid(111111) << "\n";
	std::cout << "223450: " << password::isValid(223450) << "\n";
	std::cout << "123789: " << password::isValid(123789) << "\n";
	std::cout << "112233: " << password::isValid(112233) << "\n";
	std::cout << "123444: " << password::isValid(123444) << "\n";
	std::cout << "111122: " << password::isValid(111122) << "\n";
	
	const size_t first = 130254;
	const size_t last = 678275;
	auto buffer = std::vector<size_t>{};

	size_t cpt = 0;
	for (size_t number = first; number <= last; ++number)
		if (password::isValid(number, buffer))
			++cpt;
	std::cout << "N passwords: " << cpt << "\n";

	std::cin.get();
}
