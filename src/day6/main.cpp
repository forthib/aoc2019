#include "orbits.h"

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

	std::vector<std::array<std::string, 2>> read(const std::string& filePath)
	{
		auto in = std::ifstream{ filePath };

		std::string line;
		auto values = std::vector<std::array<std::string, 2>>{};
		while (!in.eof())
		{
			std::getline(in, line);
			if (!line.empty())
			{
				const auto splitLine = split(line, ')');
				values.push_back({ splitLine[0], splitLine[1] });
			}
		}
		return values;
	}
}

int main(int argc, char* argv[])
{
	const auto test1 = read(R"(C:\TFO\REPOS\aoc2019\data\day6\test1.txt)");
	const auto test2 = read(R"(C:\TFO\REPOS\aoc2019\data\day6\test2.txt)");
	const auto input = read(R"(C:\TFO\REPOS\aoc2019\data\day6\input.txt)");

	std::cout << orbits::count(test1) << "\n";
	std::cout << orbits::count(input) << "\n";
	std::cout << orbits::shortest(test2, "YOU", "SAN") << "\n";
	std::cout << orbits::shortest(input, "YOU", "SAN") << "\n";

	std::cin.get();
}
