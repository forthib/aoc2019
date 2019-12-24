#include "image.h"
#include "io.h"
#include "test.h"
#include <unordered_map>
#include <unordered_set>

using namespace image;

namespace {
	Image<char> read(const std::string& fileName)
	{
		const auto lines  = io::readLinesOfChars(fileName);
		const auto height = lines.size();
		const auto width  = lines[0].size();

		auto state = Image<char>(width, height);
		for (size_t j = 0; j < height; ++j)
			for (size_t i = 0; i < width; ++i)
				state(i, j) = lines[j][i];
		return state;
	}

	size_t countAdjacentBugs(const Image<char>& state, size_t i, size_t j)
	{
		size_t n = 0;
		if (i > 0 && state(i - 1, j) == '#')
			++n;
		if (i < state.getWidth() - 1 && state(i + 1, j) == '#')
			++n;
		if (j > 0 && state(i, j - 1) == '#')
			++n;
		if (j < state.getHeight() - 1 && state(i, j + 1) == '#')
			++n;
		return n;
	}

	size_t countAdjacentBugs(const std::unordered_map<int64_t, Image<char>>& states, int64_t level, size_t i, size_t j)
	{
		const auto* state      = states.count(level) > 0 ? &(states.at(level)) : nullptr;
		const auto* statePlus  = states.count(level + 1) > 0 ? &(states.at(level + 1)) : nullptr;
		const auto* stateMinus = states.count(level - 1) > 0 ? &(states.at(level - 1)) : nullptr;

		size_t n = 0;

		if (state)
			n += countAdjacentBugs(*state, i, j);

		if (i == 2 && j == 1 && statePlus) {
			for (size_t x = 0; x < 5; ++x)
				if ((*statePlus)(x, 0) == '#')
					++n;
		}

		if (i == 2 && j == 3 && statePlus) {
			for (size_t x = 0; x < 5; ++x)
				if ((*statePlus)(x, 4) == '#')
					++n;
		}

		if (i == 1 && j == 2 && statePlus) {
			for (size_t y = 0; y < 5; ++y)
				if ((*statePlus)(0, y) == '#')
					++n;
		}

		if (i == 3 && j == 2 && statePlus) {
			for (size_t y = 0; y < 5; ++y)
				if ((*statePlus)(4, y) == '#')
					++n;
		}

		if (i == 0 && stateMinus && (*stateMinus)(1, 2) == '#')
			++n;

		if (i == 4 && stateMinus && (*stateMinus)(3, 2) == '#')
			++n;

		if (j == 0 && stateMinus && (*stateMinus)(2, 1) == '#')
			++n;

		if (j == 4 && stateMinus && (*stateMinus)(2, 3) == '#')
			++n;

		return n;
	}

	Image<char> next(const Image<char>& state)
	{
		auto result = state;
		for (size_t j = 0; j < state.getHeight(); ++j) {
			for (size_t i = 0; i < state.getWidth(); ++i) {
				const auto nBugs = countAdjacentBugs(state, i, j);
				if (state(i, j) == '#') {
					if (nBugs != 1)
						result(i, j) = '.';
				}
				else {
					if (nBugs == 1 || nBugs == 2)
						result(i, j) = '#';
				}
			}
		}
		return result;
	}

	Image<char> next(Image<char> state, size_t n)
	{
		for (size_t i = 0; i < n; ++i)
			state = next(state);
		return state;
	}

	Image<char> next(const std::string& fileName, size_t n) { return next(read(fileName), n); }

	Image<char> next(const std::unordered_map<int64_t, Image<char>>& states, int64_t level)
	{
		auto nextState = states.count(level) > 0 ? states.at(level) : Image<char>{ 5, 5, '.' };
		for (size_t j = 0; j < 5; ++j) {
			for (size_t i = 0; i < 5; ++i) {
				if (i == 2 && j == 2)
					continue;
				
				const auto nBugs = countAdjacentBugs(states, level, i, j);
				if (nextState(i, j) == '#') {
					if (nBugs != 1)
						nextState(i, j) = '.';
				}
				else {
					if (nBugs == 1 || nBugs == 2)
						nextState(i, j) = '#';
				}
			}
		}
		return nextState;
	}

	std::unordered_map<int64_t, Image<char>> removedEmpty(const std::unordered_map<int64_t, Image<char>>& states)
	{
		auto result = std::unordered_map<int64_t, Image<char>>{};
		for (auto&& entry : states)
			if (std::count(entry.second.begin(), entry.second.end(), '#') > 0)
				result[entry.first] = entry.second;
		return result;
	}

	std::unordered_map<int64_t, Image<char>> next(const std::unordered_map<int64_t, Image<char>>& states)
	{
		auto result = std::unordered_map<int64_t, Image<char>>{};
		for (auto&& entry : states) {
			const auto level = entry.first;
			result[level]    = next(states, level);

			if (states.count(level - 1) == 0)
				result[level - 1] = next(states, level - 1);
			if (states.count(level + 1) == 0)
				result[level + 1] = next(states, level + 1);
		}
		return removedEmpty(result);
	}

	size_t computeBiodiversity(const Image<char>& state)
	{
		size_t biodiversity = 0;
		for (auto it = state.rbegin(); it != state.rend(); ++it)
			biodiversity = (biodiversity << 1) + (*it == '#' ? 1 : 0);
		return biodiversity;
	}

	size_t computeBiodiversity(const std::string& fileName) { return computeBiodiversity(read(fileName)); }

	size_t computeNBugs(const std::unordered_map<int64_t, Image<char>>& states)
	{
		size_t n = 0;
		for (auto&& entry : states) {
			const auto& state = entry.second;
			n += std::count(state.begin(), state.end(), '#');
		}
		return n;
	}

	void runPart1()
	{
		auto state          = read("day24_input.txt");
		auto biodiversities = std::unordered_set<size_t>{};

		while (biodiversities.insert(computeBiodiversity(state)).second)
			state = next(state);

		std::cout << "Part 1: " << computeBiodiversity(state) << "\n";
	}

	void runPart2()
	{
		auto states = std::unordered_map<int64_t, Image<char>>{};
		states[0]   = read("day24_input.txt");

		for (size_t i = 0; i < 200; ++i)
			states = next(states);

		std::cout << "Part 2: " << computeNBugs(states) << "\n";
	}
}

int main(int argc, char* argv[])
{
	test::equals(next(read("day24_test1a.txt"), 4), read("day24_test1b.txt"));
	test::equals(computeBiodiversity("day24_test2.txt"), 2129920);

	runPart1();
	runPart2();

	std::cin.get();
}
