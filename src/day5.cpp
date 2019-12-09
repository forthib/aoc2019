#include "io.h"
#include "opcode.h"
#include "test.h"

int main(int argc, char* argv[])
{
	const auto codeEqual8PosMode = std::vector<std::int64_t>{ 3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8 };
	test::equals(opcode::run(codeEqual8PosMode, { 7 }), { 0 });
	test::equals(opcode::run(codeEqual8PosMode, { 8 }), { 1 });

	const auto codeLessThan8PosMode = std::vector<std::int64_t>{ 3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8 };
	test::equals(opcode::run(codeLessThan8PosMode, { 7 }), { 1 });
	test::equals(opcode::run(codeLessThan8PosMode, { 8 }), { 0 });
	test::equals(opcode::run(codeLessThan8PosMode, { 9 }), { 0 });

	const auto codeEqual8ImmMode = std::vector<std::int64_t>{ 3, 3, 1108, -1, 8, 3, 4, 3, 99 };
	test::equals(opcode::run(codeEqual8ImmMode, { 7 }), { 0 });
	test::equals(opcode::run(codeEqual8ImmMode, { 8 }), { 1 });

	const auto codeLessThan8ImmMode = std::vector<std::int64_t>{ 3, 3, 1107, -1, 8, 3, 4, 3, 99 };
	test::equals(opcode::run(codeLessThan8ImmMode, { 7 }), { 1 });
	test::equals(opcode::run(codeLessThan8ImmMode, { 8 }), { 0 });
	test::equals(opcode::run(codeLessThan8ImmMode, { 9 }), { 0 });

	const auto codeJumpPosMode = std::vector<std::int64_t>{ 3, 12, 6, 12, 15, 1, 13, 14, 13, 4, 13, 99, -1, 0, 1, 9 };
	test::equals(opcode::run(codeJumpPosMode, { 0 }), { 0 });
	test::equals(opcode::run(codeJumpPosMode, { 5 }), { 1 });

	const auto codeJumpImmMode = std::vector<std::int64_t>{ 3, 3, 1105, -1, 9, 1101, 0, 0, 12, 4, 12, 99, 1 };
	test::equals(opcode::run(codeJumpImmMode, { 0 }), { 0 });
	test::equals(opcode::run(codeJumpImmMode, { 5 }), { 1 });

	const auto codeLargerExample = std::vector<std::int64_t>{ 3, 21, 1008, 21, 8, 20, 1005, 20, 22, 107, 8, 21, 20, 1006, 20, 31, 1106, 0, 36, 98, 0, 0, 1002,
		21, 125, 20, 4, 20, 1105, 1, 46, 104, 999, 1105, 1, 46, 1101, 1000, 1, 20, 4, 20, 1105, 1, 46, 98, 99 };
	test::equals(opcode::run(codeLargerExample, { 7 }), { 999 });
	test::equals(opcode::run(codeLargerExample, { 8 }), { 1000 });
	test::equals(opcode::run(codeLargerExample, { 9 }), { 1001 });

	const auto code = io::readLineOfIntegers("day5_input.txt");
	std::cout << "Part 1: " << io::toString(opcode::run(code, { 1 })) << "\n";
	std::cout << "Part 2: " << io::toString(opcode::run(code, { 5 })) << "\n";
	std::cin.get();
}
