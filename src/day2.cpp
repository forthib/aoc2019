#include "opcode.h"
#include "io.h"

int main(int argc, char* argv[])
{
	io::display(std::cout, opcode::decode({ 1, 0, 0, 0, 99 }));
	io::display(std::cout, opcode::decode({ 2, 3, 0, 3, 99 }));
	io::display(std::cout, opcode::decode({ 2, 4, 4, 5, 99, 0 }));
	io::display(std::cout, opcode::decode({ 1, 1, 1, 4, 99, 5, 6, 0, 99 }));

	const auto originalCode = io::readLineOfIntegers("day2_input.txt");

	{
		auto code = originalCode;
		code[1] = 12;
		code[2] = 2;
		code = opcode::decode(std::move(code));
		std::cout << "Result: " << code.front() << "\n";
	}

	for (int noun = 0; noun <= 100; ++noun) {
		for (int verb = 0; verb <= 100; ++verb) {
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
