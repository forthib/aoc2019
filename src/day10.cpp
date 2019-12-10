#include "asteroid.h"
#include "test.h"

int main(int argc, char* argv[])
{
	test::equals(asteroid::getMaxNumberOfDetections("day10_test1.txt"), 8);
	test::equals(asteroid::getMaxNumberOfDetections("day10_test2.txt"), 33);
	test::equals(asteroid::getMaxNumberOfDetections("day10_test3.txt"), 35);
	test::equals(asteroid::getMaxNumberOfDetections("day10_test4.txt"), 41);
	test::equals(asteroid::getMaxNumberOfDetections("day10_test5.txt"), 210);

	std::cout << "Part 1: " << asteroid::getMaxNumberOfDetections("day10_input.txt") << "\n";

	const auto destroyedIndexes = asteroid::destroyAsteroids("day10_test5.txt");
	test::equals(destroyedIndexes[0], asteroid::IJ{ 11, 12 });
	test::equals(destroyedIndexes[1], asteroid::IJ{ 12, 1 });
	test::equals(destroyedIndexes[2], asteroid::IJ{ 12, 2 });
	test::equals(destroyedIndexes[9], asteroid::IJ{ 12, 8 });
	test::equals(destroyedIndexes[19], asteroid::IJ{ 16, 0 });
	test::equals(destroyedIndexes[49], asteroid::IJ{ 16, 9 });
	test::equals(destroyedIndexes[99], asteroid::IJ{ 10, 16 });
	test::equals(destroyedIndexes[198], asteroid::IJ{ 9, 6 });
	test::equals(destroyedIndexes[199], asteroid::IJ{ 8, 2 });
	test::equals(destroyedIndexes[200], asteroid::IJ{ 10, 9 });
	test::equals(destroyedIndexes[298], asteroid::IJ{ 11, 1 });

	const auto index200 = asteroid::destroyAsteroids("day10_input.txt")[199];
	std::cout << "Part 2: " << (index200.i * 100 + index200.j) << "\n";

	std::cin.get();
}
