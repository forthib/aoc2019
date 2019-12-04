#include "opcode.h"

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

	std::vector<size_t> read(const std::string& filePath)
	{
		auto in = std::ifstream{ filePath };

		std::string line;
		std::getline(in, line);

		const auto strValues = split(line, ',');

		auto values = std::vector<size_t>{};
		for (const auto& strValue : strValues)
			values.push_back(std::atoi(strValue.c_str()));
		return values;
	}

	void display(std::ostream& out, const std::vector<size_t>& values)
	{
		out << "[";
		if (!values.empty())
		{
			out << values[0];
			for (size_t i = 1; i < values.size(); ++i)
				out << ", " << values[i];
		}
		out << "]\n";
	}
}

int main(int argc, char* argv[])
{
	display(std::cout, opcode::decode({ 1, 0, 0, 0, 99 }));
	display(std::cout, opcode::decode({ 2, 3, 0, 3, 99 }));
	display(std::cout, opcode::decode({ 2, 4, 4, 5, 99, 0 }));
	display(std::cout, opcode::decode({ 1, 1, 1, 4, 99, 5, 6, 0, 99 }));

	const auto originalCode = read(R"(C:\TFO\REPOS\aoc2019\data\day2\input.txt)");

	{
		auto code = originalCode;
		code[1] = 12;
		code[2] = 2;
		code = opcode::decode(std::move(code));
		std::cout << "Result: " << code.front() << "\n";
	}

	for (size_t noun = 0; noun <= 100; ++noun) {
		for (size_t verb = 0; verb <= 100; ++verb) {
			auto code = originalCode;
			code[1] = noun;
			code[2] = verb;
			try
			{
				code = opcode::decode(std::move(code));
			}
			catch (const std::exception&)
			{
				continue;
			}
			if (code.front() == 19690720)
			{	
				std::cout << "Noun: " << noun << " verb: " << verb << "\n";
				std::cout << "100 * noun + verb: " << (100 * noun + verb) << "\n";
			}
		}
	}

	std::cin.get();
}
