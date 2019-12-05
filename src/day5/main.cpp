#include "opcode2.h"

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

	std::vector<int> read(const std::string& filePath)
	{
		auto in = std::ifstream{ filePath };

		std::string line;
		std::getline(in, line);

		const auto strValues = split(line, ',');

		auto values = std::vector<int>{};
		for (const auto& strValue : strValues)
			values.push_back(std::atoi(strValue.c_str()));
		return values;
	}

	void display(std::ostream& out, const std::vector<int>& values)
	{
		out << "[";
		if (!values.empty())
		{
			out << values[0];
			for (int i = 1; i < values.size(); ++i)
				out << ", " << values[i];
		}
		out << "]\n";
	}
}

int main(int argc, char* argv[])
{
	//opcode2::decode({ 3, 0, 4, 0, 99 });
	//opcode2::decode({ 1002, 4, 3, 4, 33 });
	//
	//opcode2::decode({ 3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8 });
	//opcode2::decode({ 3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8 });
	//opcode2::decode({ 3, 3, 1108, -1, 8, 3 ,4, 3, 99 });
	//opcode2::decode({ 3, 3, 1107, -1, 8, 3, 4, 3, 99 });
	//
	//opcode2::decode({ 3, 12, 6, 12, 15, 1, 13, 14, 13, 4, 13, 99, -1, 0, 1, 9 });
	//opcode2::decode({ 3, 3, 1105, -1, 9, 1101, 0, 0, 12, 4, 12, 99, 1 });
	//
	//opcode2::decode({ 3, 21, 1008, 21, 8, 20, 1005, 20, 22, 107, 8, 21, 20, 1006, 20, 31, 1106, 0, 36, 98, 0, 0, 1002, 21, 125, 20, 4, 20, 1105, 1, 46, 104, 999, 1105, 1, 46, 1101, 1000, 1, 20, 4, 20, 1105, 1, 46, 98, 99 });

	auto code = read(R"(C:\TFO\REPOS\aoc2019\data\day5\input.txt)");

	code = opcode2::decode(std::move(code));

	std::cin.get();
	std::cin.get();
}
