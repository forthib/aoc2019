#include "io.h"
#include "opcode.h"
#include "test.h"

namespace {
	void check(std::vector<std::int64_t> code, const std::vector<std::int64_t>& expectedCode)
	{
		opcode::run(code);
		test::equals(code, expectedCode);
	}

	void runPart1(std::vector<std::int64_t> code)
	{
		code[1] = 12;
		code[2] = 2;
		opcode::run(code);
		std::cout << "Part1: " << code.front() << "\n";
	}

	void runPart2(const std::vector<std::int64_t>& originalCode)
	{

		for (int noun = 0; noun <= 100; ++noun) {
			for (int verb = 0; verb <= 100; ++verb) {
				auto code = originalCode;
				code[1]   = noun;
				code[2]   = verb;
				try {
					opcode::run(code);
				}
				catch (const std::exception&) {
					continue;
				}
				if (code.front() == 19690720)
					std::cout << "Part2: " << (100 * noun + verb) << "\n";
			}
		}
	}
}

int main(int argc, char* argv[])
{
	check({ 1, 0, 0, 0, 99 }, { 2, 0, 0, 0, 99 });
	check({ 2, 3, 0, 3, 99 }, { 2, 3, 0, 6, 99 });
	check({ 2, 4, 4, 5, 99, 0 }, { 2, 4, 4, 5, 99, 9801 });
	check({ 1, 1, 1, 4, 99, 5, 6, 0, 99 }, { 30, 1, 1, 4, 2, 5, 6, 0, 99 });

	const auto code = io::readLineOfIntegers("day2_input.txt");
	runPart1(code);
	runPart2(code);

	std::cin.get();
}
