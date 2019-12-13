#include "image.h"
#include "io.h"
#include "opcode.h"

namespace {
	void display(const image::Image<std::int64_t>& screen)
	{
		const auto pretty = image::transform<char>(screen, [](const auto value) {
			switch (value) {
			case 0: return ' ';
			case 1: return '|';
			case 2: return '#';
			case 3: return '_';
			case 4: return 'o';
			default: throw std::exception{ "unsupported value" };
			}
		});
		display(pretty);
	}
}

int main(int argc, char* argv[])
{
	auto         screen = image::Image<std::int64_t>{ 0, 0 };
	std::int64_t score  = 0;

	const auto inputFunction = [&]() {
		const auto playerIndex = std::distance(screen.begin(), std::find(screen.begin(), screen.end(), 3));
		const auto ballIndex   = std::distance(screen.begin(), std::find(screen.begin(), screen.end(), 4));
		const auto playerI     = screen.getIJ(playerIndex).i;
		const auto ballI       = screen.getIJ(ballIndex).i;

		if (playerI < ballI)
			return 1;
		else if (playerI > ballI)
			return -1;
		else
			return 0;
	};

	auto       outputs        = std::vector<std::int64_t>{};
	const auto outputFunction = [&](std::int64_t value) {
		outputs.push_back(value);
		if (outputs.size() == 3) {
			if (outputs[0] == -1 && outputs[1] == 0)
				score = outputs[2];
			else {
				const auto position = image::IJ{ static_cast<size_t>(outputs[0]), static_cast<size_t>(outputs[1]) };
				if (position.i >= screen.getWidth())
					resize(screen, position.i + 1, screen.getHeight());
				if (position.j >= screen.getHeight())
					resize(screen, screen.getWidth(), position.j + 1);
				screen(position) = outputs[2];
			}
			outputs.clear();
		}
	};

	const auto code = io::readLineOfIntegers("day13_input.txt");
	opcode::run(code, inputFunction, outputFunction);
	display(screen);

	const auto nBlocks = std::count(screen.begin(), screen.end(), 2);
	std::cout << "Part 1: " << nBlocks << "\n";

	auto code2 = io::readLineOfIntegers("day13_input.txt");
	code2[0]   = 2;
	opcode::run(code2, inputFunction, outputFunction);
	std::cout << "Part 2: " << score << "\n";

	std::cin.get();
}
