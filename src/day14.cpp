#include "reaction.h"
#include "test.h"

int main(int argc, char* argv[])
{
	test::equals(countRequestedQuantity(reaction::readReactions("day14_test1.txt")), 31);
	test::equals(countRequestedQuantity(reaction::readReactions("day14_test2.txt")), 165);
	test::equals(countRequestedQuantity(reaction::readReactions("day14_test3.txt")), 13312);
	test::equals(countRequestedQuantity(reaction::readReactions("day14_test4.txt")), 180697);
	test::equals(countRequestedQuantity(reaction::readReactions("day14_test5.txt")), 2210736);

	test::equals(countProducedQuantity(reaction::readReactions("day14_test3.txt")), 82892753);
	test::equals(countProducedQuantity(reaction::readReactions("day14_test4.txt")), 5586022);
	test::equals(countProducedQuantity(reaction::readReactions("day14_test5.txt")), 460664);

	std::cout << "Part 1: " << countRequestedQuantity(reaction::readReactions("day14_input.txt")) << "\n";
	std::cout << "Part 2: " << countProducedQuantity(reaction::readReactions("day14_input.txt")) << "\n";

	std::cin.get();
}