#include "image.h"
#include "io.h"

int main(int argc, char* argv[])
{
	const auto data = io::readLineOfDigits("day8_input.txt");

	std::cout << "Part 1 test - raw image:\n";
	for (const auto& layer : image::splitLayers(3, 2, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2 })) {
		std::cout << "\n";
		display(layer, std::cout, "\t");
	}
	std::cout << "\n";

	const auto layers = image::splitLayers(25, 6, data);
	std::cout << "Part 1 - check value: " << getCheckValue(layers) << "\n\n";

	std::cout << "Part 2 test - decoded image:\n\n";
	display(decode(image::splitLayers(2, 2, { 0, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 2, 0, 0, 0, 0 })), std::cout, "\t");
	std::cout << "\n";
	
	std::cout << "Part 2 - decoded image:\n\n";
	const auto decoded = decode(layers);
	const auto pretty = image::transform<char>(decoded, [](const auto value) { return value ? '#' : ' '; });
	display(pretty, std::cout, "\t");
	std::cout << "\n";

	std::cin.get();
}
