#include "fuel.h"
#include "io.h"

int main(int argc, char* argv[])
{
	const auto input = io::readIntegers("day1_input.txt");

	size_t totalFuelSimple = 0;
	size_t totalFuel = 0;
	for (const size_t mass : input)
	{
		totalFuelSimple += fuel::fromMass(mass, false);
		totalFuel += fuel::fromMass(mass, true);
	}

	std::cout << "14 -> " << fuel::fromMass(14, true) << "\n";
	std::cout << "1969 -> " << fuel::fromMass(1969, true) << "\n";
	std::cout << "100756 -> " << fuel::fromMass(100756, true) << "\n";
	std::cout << "Total fuel (simple) = " << totalFuelSimple << "\n";
	std::cout << "Total fuel = " << totalFuel << "\n";
	std::cin.get();
}
