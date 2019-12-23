#include "io.h"
#include "test.h"
#include <boost/multiprecision/cpp_int.hpp>

namespace {

	using Int = boost::multiprecision::int128_t;

	Int modMult(const Int& a, const Int& b, const Int& n) { return (a * b) % n; }

	Int modInv(Int a, Int n)
	{
		if (n == 1)
			return 1;

		const auto n0 = n;
		Int        x0 = 0, x1 = 1;
		while (a > 1) {
			const auto q = a / n;
			auto       t = n;

			n  = a % n;
			a  = t;
			t  = x0;
			x0 = x1 - q * x0;
			x1 = t;
		}
		if (x1 < 0)
			x1 += n0;
		return x1;
	}

	Int modDiv(const Int& a, const Int& b, const Int& n) { return (a * modInv(b, n)) % n; }

	struct Instruction
	{
		Int a, b;
	};

	std::vector<Instruction> convert(const std::vector<std::vector<std::string>>& instructions, Int n)
	{
		auto result = std::vector<Instruction>{};
		for (const auto& instruction : instructions) {
			if (instruction[0] == "deal" && instruction[1] == "into")
				result.push_back({ n - 1, 1 });
			else if (instruction[0] == "deal" && instruction[1] == "with") {
				const auto value = Int{ std::atoll(instruction.back().c_str()) };
				result.push_back({ value, 0 });
			}
			else if (instruction[0] == "cut") {
				auto value = Int{ std::atoll(instruction.back().c_str()) };
				if (value < 0)
					value += n;
				result.push_back({ 1, value });
			}
		}
		return result;
	}

	std::vector<Instruction> read(const std::string& fileName, const Int& n)
	{ //
		return convert(io::readLinesOfStrings(fileName, " "), n);
	}

	Instruction reduced(const Instruction& instruction1, const Instruction& instruction2, const Int& n)
	{
		const auto a1 = instruction1.a;
		const auto a2 = instruction2.a;
		const auto b1 = instruction1.b;
		const auto b2 = instruction2.b;
		const auto a  = modMult(a1, a2, n);
		const auto b  = (modMult(a2, b1, n) + b2) % n;
		return { a, b };
	}

	Instruction reduced(const std::vector<Instruction>& instructions, const Int& n)
	{
		auto instruction = Instruction{ 1, 0 };
		for (const auto& ins : instructions)
			instruction = reduced(instruction, ins, n);
		return instruction;
	}

	Int apply(const Instruction& instruction, const Int& n, Int pos)
	{
		pos = (pos * instruction.a) % n;
		pos = (pos + (n - instruction.b)) % n;
		return pos;
	}

	Int applyInv(const Instruction& instruction, const Int& n, Int pos)
	{
		pos = (pos + instruction.b) % n;
		pos = modDiv(pos, instruction.a, n);
		return pos;
	}

	std::vector<Int> apply(const Instruction& instruction, const std::vector<Int>& input)
	{
		const auto n     = Int{ input.size() };
		auto       cards = std::vector<Int>(input.size());
		for (Int i = 0; i < n; ++i)
			cards[apply(instruction, n, i).convert_to<uint64_t>()] = input[i.convert_to<uint64_t>()];
		return cards;
	}

	std::vector<Int> apply(const Instruction& instruction, Int n)
	{
		auto cards = std::vector<Int>(n.convert_to<size_t>());
		for (Int i = 0; i < n; ++i)
			cards[apply(instruction, n, i).convert_to<uint64_t>()] = i;
		return cards;
	}

	std::vector<Int> applyInv(const Instruction& instruction, const std::vector<Int>& input)
	{
		const auto n     = Int{ input.size() };
		auto       cards = std::vector<Int>(input.size());
		for (Int i = 0; i < n; ++i)
			cards[applyInv(instruction, n, i).convert_to<uint64_t>()] = input[i.convert_to<uint64_t>()];
		return cards;
	}

	std::vector<Int> applyInv(const Instruction& instruction, Int n)
	{
		auto cards = std::vector<Int>(n.convert_to<size_t>());
		for (Int i = 0; i < n; ++i)
			cards[applyInv(instruction, n, i).convert_to<uint64_t>()] = i;
		return cards;
	}

	std::vector<Int> apply(const std::vector<Instruction>& instructions, Int n) { return apply(reduced(instructions, n), n); }

	Instruction reducedN(Instruction instruction, const Int& nCards, Int nTimes)
	{
		auto exponent = instruction;
		instruction   = Instruction{ 1, 0 };
		while (nTimes > 0) {
			if (nTimes % 2 == 1)
				instruction = reduced(instruction, exponent, nCards);
			exponent = reduced(exponent, exponent, nCards);
			nTimes   = nTimes >> 1;
		}
		return instruction;
	}

	void runPart1()
	{
		const auto nCards      = 10007;
		const auto instruction = reduced(read("day22_input.txt", nCards), nCards);
		const auto pos2019     = apply(instruction, nCards, 2019);
		std::cout << "Part 1: " << pos2019 << "\n";
		test::equals(applyInv(instruction, nCards, pos2019), 2019);
	}

	void runPart2()
	{
		const auto nCards = 119315717514047;
		const auto nTimes = 101741582076661;

		auto instruction = reduced(read("day22_input.txt", nCards), nCards);
		instruction      = reducedN(instruction, nCards, nTimes);

		const auto pos2020 = applyInv(instruction, nCards, 2020);
		std::cout << "Part 2: " << pos2020 << "\n";
		test::equals(apply(instruction, nCards, pos2020), 2020);
	}
}

int main(int argc, char* argv[])
{
	test::equals(apply(read("day22_test1.txt", 10), 10), std::vector<Int>{ 0, 3, 6, 9, 2, 5, 8, 1, 4, 7 });
	test::equals(apply(read("day22_test2.txt", 10), 10), std::vector<Int>{ 3, 0, 7, 4, 1, 8, 5, 2, 9, 6 });
	test::equals(apply(read("day22_test3.txt", 10), 10), std::vector<Int>{ 6, 3, 0, 7, 4, 1, 8, 5, 2, 9 });
	test::equals(apply(read("day22_test4.txt", 10), 10), std::vector<Int>{ 9, 2, 5, 8, 1, 4, 7, 0, 3, 6 });

	runPart1();
	runPart2();

	std::cin.get();
}