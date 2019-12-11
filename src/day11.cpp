#include "image.h"
#include "io.h"
#include "opcode.h"
#include "robot.h"

#include <unordered_map>

namespace {

	robot::PaintingRobot run(std::vector<std::int64_t> code, int64_t firstValue)
	{
		auto paintingRobot = robot::PaintingRobot{ firstValue };

		const auto inputFunction  = [&]() { return paintingRobot.getCurrentValue(); };
		const auto outputFunction = [&](std::int64_t value) { paintingRobot.decode(value); };
		opcode::run(code, inputFunction, outputFunction);

		return paintingRobot;
	}
}

int main(int argc, char* argv[])
{
	const auto code = io::readLineOfIntegers("day11_input.txt");

	const auto robot1 = run(code, 0);
	const auto robot2 = run(code, 1);

	std::cout << "Part1: " << robot1.getNPaintedPanels() << "\n\n";
	
	const auto image = image::transform<char>(robot2.getImage(), [](auto value) { return value ? '#' : ' '; });
	std::cout << "Part2:\n\n";
	display(image, std::cout, "\t");

	std::cin.get();
}
