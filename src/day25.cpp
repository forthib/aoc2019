#include "io.h"
#include "opcode.h"

int main(int argc, char* argv[])
{
	const auto code  = io::readLineOfIntegers("day25_input.txt");
	const auto items = std::vector<std::string>{ "prime number", "candy cane", "asterisk", "food ration", "boulder", "mutex", "mug", "loom" };
	const auto mask  = 154;

	auto inputs = std::vector<std::string>{ "south", "west", "take asterisk", "east", "take boulder", "east", "take food ration", "west", "north",
		"east", "take candy cane", "north", "east", "north", "take mug", "south", "west", "north", "take mutex", "north", "take prime number", "south", "south",
		"south", "east", "north", "take loom", "south", "east", "south", "east", "east" };

	for (auto i = 0; i < 8; ++i) {
		if ((mask >> i) & 1)
			inputs.push_back("drop " + items[i]);
	}
	inputs.emplace_back("inv");
	inputs.emplace_back("north");

	std::string input;

	const auto inputFunction = [&]() {
		if (input.empty()) {
			if (!inputs.empty()) {
				input = inputs.front();
				inputs.erase(inputs.begin());
				std::cout << input << "\n";
			}
			else {
				std::getline(std::cin, input);
			}
			input += '\n';
		}

		const auto value = static_cast<int64_t>(input.front());
		input.erase(0, 1);
		return value;
	};

	std::string output;

	const auto ouputFunction = [&](int64_t value) { std::cout << static_cast<char>(value); };

	opcode::run(code, inputFunction, ouputFunction);

	std::cin.get();
}
