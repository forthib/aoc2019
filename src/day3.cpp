#include "intersection.h"
#include "io.h"

namespace
{
	void run(const std::vector<std::string>& wire1, const std::vector<std::string>& wire2)
	{
		std::cout << "Distance: " << intersection::distance(wire1, wire2) << " steps: " << intersection::steps(wire1, wire2) << "\n";
	}
}

int main(int argc, char* argv[])
{
	run(io::split("R8,U5,L5,D3"), io::split("U7,R6,D4,L4"));
	run(io::split("R75,D30,R83,U83,L12,D49,R71,U7,L72"), io::split("U62,R66,U55,R34,D71,R55,D58,R83"));
	run(io::split("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51"), io::split("U98,R91,D20,R16,D67,R40,U7,R15,U6,R7"));

	const auto wires = io::readLinesOfStrings("day3_input.txt");
	run(wires[0], wires[1]);

	std::cin.get();
}
