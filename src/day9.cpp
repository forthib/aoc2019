#include "io.h"
#include "opcode.h"
#include "test.h"

int main(int argc, char* argv[])
{
	const auto codeQuine = std::vector<std::int64_t>{ 109, 1, 204, -1, 1001, 100, 1, 100, 1008, 100, 16, 101, 1006, 101, 0, 99 };
	test::equals(opcode::run(codeQuine), codeQuine);
	test::equals(opcode::run({ 1102, 34915192, 34915192, 7, 4, 7, 99, 0 }), { 1219070632396864 });
	test::equals(opcode::run({ 104, 1125899906842624, 99 }), { 1125899906842624 });

	const auto code = io::readLineOfIntegers("day9_input.txt");
	std::cout << "Part 1: " << io::toString(opcode::run(code, { 1 })) << "\n";
	std::cout << "Part 2: " << io::toString(opcode::run(code, { 2 })) << "\n";

	std::cin.get();
}
