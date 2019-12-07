#include "io.h"

#include <fstream>

namespace io
{
	namespace
	{
		std::string getFilePath(const std::string& fileName)
		{
			return R"(C:\TFO\REPOS\aoc2019\data\)" + fileName;
		}

		bool contains(const std::string& s, char c)
		{
			return std::find(s.begin(), s.end(), c) != s.end();
		}
	}

	std::vector<std::string> split(const std::string& s, const std::string& separators)
	{
		std::vector<std::string> result;

		for (auto it = begin(s);; ++it)
		{
			auto first = it;
			while (it != end(s) && !contains(separators, *it))
				++it;

			result.emplace_back(first, it);

			if (it == end(s))
				break;
		}

		return result;
	}

	std::vector<int> readIntegers(const std::string& fileName)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		auto values = std::vector<int>{};
		while (!in.eof())
		{
			int value;
			if (in >> value)
				values.push_back(value);
		}
		return values;
	}

	std::vector<int> readLineOfIntegers(const std::string& fileName, const std::string& separators)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		std::string line;
		std::getline(in, line);

		const auto strValues = split(line, separators);

		auto values = std::vector<int>{};
		for (const auto& strValue : strValues)
			values.push_back(std::atoi(strValue.c_str()));
		return values;
	}

	std::vector<std::string> readLinesOfString(const std::string& fileName)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		auto values = std::vector<std::string>{};
		while (!in.eof())
		{
			std::string line;
			std::getline(in, line);
			if (!line.empty())
				values.push_back(line);
		}
		return values;
	}

	std::vector<std::vector<std::string>> readLinesOfStrings(const std::string& fileName, const std::string& separators)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		auto values = std::vector<std::vector<std::string>>{};
		while (!in.eof())
		{
			std::string line;
			std::getline(in, line);
			if (!line.empty())
				values.push_back(split(line, separators));
		}
		return values;
	}

	void display(std::ostream& out, const std::vector<int>& values)
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