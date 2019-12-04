#include "intersection.h"

#include <fstream>
#include <iostream>
#include <string>

namespace
{
	std::vector<std::string> split(const std::string& s, char c = ' ')
	{
		std::vector<std::string> result;

		for (auto it = begin(s);; ++it)
		{
			auto first = it;
			while (it != end(s) && *it != c)
				++it;

			result.emplace_back(first, it);

			if (it == end(s))
				break;
		}

		return result;
	}

	std::array<std::vector<std::string>, 2> read(const std::string& filePath)
	{
		auto in = std::ifstream{ filePath };

		std::string line1, line2;
		std::getline(in, line1);
		std::getline(in, line2);

		const auto values1 = split(line1, ',');
		const auto values2 = split(line2, ',');

		return { values1, values2 };
	}

	void run(const std::vector<std::string>& wire1, const std::vector<std::string>& wire2)
	{
		std::cout << "Distance: " << intersection::distance(wire1, wire2) << " steps: " << intersection::steps(wire1, wire2) << "\n";
	}
}

int main(int argc, char* argv[])
{
	run(split("R8,U5,L5,D3", ','), split("U7,R6,D4,L4", ','));
	run(split("R75,D30,R83,U83,L12,D49,R71,U7,L72", ','), split("U62,R66,U55,R34,D71,R55,D58,R83", ','));
	run(split("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51", ','), split("U98,R91,D20,R16,D67,R40,U7,R15,U6,R7", ','));

	const auto wires = read(R"(C:\TFO\REPOS\aoc2019\data\day3\input.txt)");
	run(wires[0], wires[1]);

	std::cin.get();
}
