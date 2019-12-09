#include "image.h"
#include "io.h"

int main(int argc, char* argv[])
{
	const auto data = io::readLineOfDigits("day8_input.txt");

	image::displayRaw(3, 2, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2 });
	image::check(25, 6, data);

	image::displayDecoded(2, 2, { 0, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 2, 0, 0, 0, 0 });
	image::displayDecoded(25, 6, data, true);

	std::cin.get();
}
