#include "orbits.h"
#include "io.h"

namespace
{
	std::vector<std::array<std::string, 2>> read(const std::string& fileName)
	{
		auto values = std::vector<std::array<std::string, 2>>{};
		for (const auto& line : io::readLinesOfString(fileName))
		{
			const auto splitLine = io::split(line, ")");
			values.push_back({ splitLine[0], splitLine[1] });
		}
		return values;
	}
}

int main(int argc, char* argv[])
{
	const auto test1 = read("day6_test1.txt");
	const auto test2 = read("day6_test2.txt");
	const auto input = read("day6_input.txt");

	std::cout << orbits::count(test1) << "\n";
	std::cout << orbits::count(input) << "\n";
	std::cout << orbits::shortest(test2, "YOU", "SAN") << "\n";
	std::cout << orbits::shortest(input, "YOU", "SAN") << "\n";

	std::cin.get();
}
