#include "image.h"
#include "io.h"
#include "opcode.h"
#include "test.h"

namespace {
	std::vector<int64_t> convert(const std::string& s)
	{
		auto result = std::vector<int64_t>{};
		for (const char c : s)
			result.push_back(c);
		return result;
	}

	void runPart1(const std::vector<int64_t>& code)
	{
		const auto inputs = convert("NOT A T\nNOT B J\nOR T J\nNOT C T\nOR T J\nAND D J\nWALK\n");
		const auto output = opcode::run(code, inputs).back();
		std::cout << "Part 1: " << output << "\n";
	}

	void runPart2(const std::vector<int64_t>& code)
	{
		const auto inputs = convert("NOT A T\nNOT B J\nOR T J\nNOT C T\nOR T J\nAND D J\nNOT E T\nNOT T T\nOR H T\nAND T J\nRUN\n");
		const auto output = opcode::run(code, inputs).back();
		std::cout << "Part 2: " << output << "\n";
	}
}

int main(int argc, char* argv[])
{
	const auto code = io::readLineOfIntegers("day21_input.txt");
	runPart1(code);
	runPart2(code);

	std::cin.get();
}
