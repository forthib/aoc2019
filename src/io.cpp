#include "io.h"

#include <fstream>
#include <sstream>

namespace io {
	namespace {
		std::string getFilePath(const std::string& fileName) { return R"(C:\TFO\REPOS\aoc2019\data\)" + fileName; }

		bool contains(const std::string& s, char c) { return std::find(s.begin(), s.end(), c) != s.end(); }
	}

	std::vector<std::string> split(const std::string& s, const std::string& separators)
	{
		std::vector<std::string> result;

		for (auto it = begin(s);; ++it) {
			auto first = it;
			while (it != end(s) && !contains(separators, *it))
				++it;

			result.emplace_back(first, it);

			if (it == end(s))
				break;
		}

		return result;
	}

	std::vector<std::int64_t> readIntegers(const std::string& fileName)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		auto values = std::vector<std::int64_t>{};
		while (!in.eof()) {
			std::int64_t value;
			if (in >> value)
				values.push_back(value);
		}
		return values;
	}

	std::vector<std::int64_t> readLineOfIntegers(const std::string& fileName, const std::string& separators)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		std::string line;
		std::getline(in, line);

		const auto strValues = split(line, separators);

		auto values = std::vector<std::int64_t>{};
		for (const auto& strValue : strValues)
			values.push_back(std::atoll(strValue.c_str()));
		return values;
	}

	std::vector<std::int64_t> readLineOfDigits(const std::string& fileName)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		std::string line;
		std::getline(in, line);

		auto values = std::vector<std::int64_t>{};
		for (const char c : line)
			values.push_back(static_cast<std::int64_t>(c - '0'));
		return values;
	}

	std::vector<std::vector<char>> readLinesOfChars(const std::string& fileName)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		auto values = std::vector<std::vector<char>>{};
		while (!in.eof()) {
			std::string line;
			std::getline(in, line);
			if (!line.empty()) {
				values.emplace_back();
				for (const char c : line)
					values.back().push_back(c);
			}
		}
		return values;
	}

	std::vector<std::string> readLinesOfString(const std::string& fileName)
	{
		auto in = std::ifstream{ getFilePath(fileName) };

		auto values = std::vector<std::string>{};
		while (!in.eof()) {
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
		while (!in.eof()) {
			std::string line;
			std::getline(in, line);
			if (!line.empty())
				values.push_back(split(line, separators));
		}
		return values;
	}

	void display(std::ostream& out, const std::vector<std::int64_t>& values, const std::string& suffix)
	{
		out << "[";
		if (!values.empty()) {
			out << values[0];
			for (size_t i = 1; i < values.size(); ++i)
				out << ", " << values[i];
		}
		out << "]" << suffix;
	}

	std::string toString(const std::vector<std::int64_t>& values)
	{
		std::ostringstream out;
		display(out, values, "");
		return out.str();
	}
}