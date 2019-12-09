#include "fuel.h"
#include "io.h"
#include "test.h"

namespace {
	std::int64_t computeFuel(const std::vector<std::int64_t>& masses, bool includeFuelMass)
	{
		size_t totalFuel = 0;
		for (const size_t mass : masses)
			totalFuel += fuel::fromMass(mass, includeFuelMass);
		return totalFuel;
	}
}

int main(int argc, char* argv[])
{
	const auto masses = io::readIntegers("day1_input.txt");
	
	std::cout << "Part 1\n";
	test::equals(fuel::fromMass(12, false), 2);
	test::equals(fuel::fromMass(14, false), 2);
	test::equals(fuel::fromMass(1969, false), 654);
	test::equals(fuel::fromMass(100756, false), 33583);
	std::cout << "Total fuel = " << computeFuel(masses, false) << "\n";
	
	std::cout << "Part 2\n";
	test::equals(fuel::fromMass(14, true), 2);
	test::equals(fuel::fromMass(1969, true), 966);
	test::equals(fuel::fromMass(100756, true), 50346);
	std::cout << "Total fuel = " << computeFuel(masses, true) << "\n";

	std::cin.get();
}
