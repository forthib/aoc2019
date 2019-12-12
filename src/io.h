#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace io {
	std::vector<std::string> split(const std::string& s, const std::string& separators = ",");

	std::vector<std::int64_t> readIntegers(const std::string& fileName);

	std::vector<std::int64_t> readLineOfIntegers(const std::string& fileName, const std::string& separators = ",");

	std::vector<std::vector<std::int64_t>> readLinesOfIntegers(const std::string& fileName, const std::string& separators = ",");

	std::vector<std::int64_t> readLineOfDigits(const std::string& fileName);

	std::vector<std::vector<char>> readLinesOfChars(const std::string& fileName);

	std::vector<std::string> readLinesOfString(const std::string& fileName, bool fullPath = false);

	std::vector<std::vector<std::string>> readLinesOfStrings(const std::string& fileName, const std::string& separators = ",", bool fullPath = false);

	void display(std::ostream& out, const std::vector<std::int64_t>& values, const std::string& suffix = "\n");

	std::string toString(const std::vector<std::int64_t>& values);
}
