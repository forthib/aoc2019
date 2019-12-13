#include "moon.h"
#include "test.h"

int main(int argc, char* argv[])
{
	test::equals(moon::getTotalEnergy("day12_test1.txt", 10), 179);
	test::equals(moon::getTotalEnergy("day12_test2.txt", 100), 1940);
	test::equals(moon::getPeriod("day12_test1.txt"), 2772);

	std::cout << "Part 1: " << moon::getTotalEnergy("day12_input.txt", 1000) << "\n";
	std::cout << "Part 2: " << moon::getPeriod("day12_input.txt") << "\n";

	std::cin.get();
}
