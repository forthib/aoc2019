#include "image.h"
#include "io.h"
#include "opcode.h"
#include "test.h"

#include <boost/optional/optional.hpp>
#include <sstream>
#include <unordered_set>

namespace {
	std::string toString(const std::vector<std::int64_t>& values)
	{
		auto result = std::string{};
		result.reserve(values.size());
		for (const auto value : values)
			result.push_back(static_cast<char>(value));
		return result;
	}

	std::vector<std::int64_t> toIntegers(const std::string& s)
	{
		auto result = std::vector<std::int64_t>{};
		result.reserve(s.size());
		for (const auto value : s)
			result.push_back(static_cast<std::int64_t>(value));
		return result;
	}

	image::Image<char> toMap(const std::string& s)
	{
		const size_t width  = std::distance(s.begin(), std::find(s.begin(), s.end(), '\n'));
		const size_t height = std::count(s.begin(), s.end(), '\n') - 1;

		auto map = image::Image<char>(width, height);
		for (size_t j = 0; j < height; ++j)
			for (size_t i = 0; i < width; ++i)
				map(i, j) = s[j * (width + 1) + i];
		return map;
	}

	bool isIntersection(const image::Image<char>& map, const image::IJ& position)
	{
		const size_t i = position.i;
		const size_t j = position.j;
		if (i == 0 || i == map.getWidth() - 1 || j == 0 || j == map.getHeight() - 1)
			return false;

		return map(i, j) == '#' && map(i - 1, j) == '#' && map(i + 1, j) == '#' && map(i, j - 1) == '#' && map(i, j + 1) == '#';
	}

	std::vector<image::IJ> getIntersections(const image::Image<char>& map)
	{
		auto intersections = std::vector<image::IJ>{};
		for (size_t j = 1; j < map.getHeight() - 1; ++j) {
			for (size_t i = 1; i < map.getWidth() - 1; ++i) {
				if (isIntersection(map, { i, j }))
					intersections.push_back({ i, j });
			}
		}
		return intersections;
	}

	image::IJ getEndPosition(const image::Image<char>& map)
	{
		for (size_t j = 0; j < map.getHeight(); ++j) {
			for (size_t i = 0; i < map.getWidth(); ++i) {
				if (map(i, j) != '#')
					continue;
				size_t count = 0;
				if (i > 0 && map(i - 1, j) == '#')
					++count;
				if (i < map.getWidth() - 1 && map(i + 1, j) == '#')
					++count;
				if (j > 0 && map(i, j - 1) == '#')
					++count;
				if (j < map.getHeight() - 1 && map(i, j + 1) == '#')
					++count;

				if (count == 1)
					return { i, j };
			}
		}
		throw std::exception{ "end position not found" };
	}

	size_t getSumOfAlignments(const image::Image<char>& map)
	{
		size_t result = 0;
		for (const auto& position : getIntersections(map))
			result += position.i * position.j;
		return result;
	}

	enum class Orientation { North, South, West, East };

	struct Command
	{
		enum Direction { Left, Right };
		Direction direction = Left;
		size_t    steps     = 0;
	};

	inline bool operator==(const Command& lhs, const Command& rhs) { return lhs.direction == rhs.direction && lhs.steps == rhs.steps; }
	inline bool operator!=(const Command& lhs, const Command& rhs) { return !operator==(lhs, rhs); }

	using CommandPath = std::vector<Command>;

	std::string toString(const Command& command)
	{
		std::ostringstream out;
		out << (command.direction == Command::Left ? 'L' : 'R') << ',' << command.steps;
		return out.str();
	}

	std::string toString(const CommandPath& path)
	{
		std::ostringstream out;
		out << toString(path.front());
		for (size_t i = 1; i < path.size(); ++i)
			out << ',' << toString(path[i]);
		return out.str();
	}

	Orientation turnedLeft(Orientation orientation)
	{
		switch (orientation) {
		case Orientation::North: return Orientation::West;
		case Orientation::West: return Orientation::South;
		case Orientation::South: return Orientation::East;
		case Orientation::East: return Orientation::North;
		default: throw std::exception{ "unsupported orientation" };
		}
	}

	Orientation turnedRight(Orientation orientation)
	{
		switch (orientation) {
		case Orientation::North: return Orientation::East;
		case Orientation::East: return Orientation::South;
		case Orientation::South: return Orientation::West;
		case Orientation::West: return Orientation::North;
		default: throw std::exception{ "unsupported orientation" };
		}
	}

	Orientation turned(Orientation orientation, Command::Direction direction)
	{
		return direction == Command::Left ? turnedLeft(orientation) : turnedRight(orientation);
	}

	image::IJ forwarded(image::IJ position, Orientation orientation, size_t steps)
	{
		switch (orientation) {
		case Orientation::North: position.j -= steps; break;
		case Orientation::South: position.j += steps; break;
		case Orientation::West: position.i -= steps; break;
		case Orientation::East: position.i += steps; break;
		default: throw std::exception{ "unsupported orientation" };
		}
		return position;
	}

	struct ExecutedPath
	{
		image::Image<bool> map;
		image::IJ          position;
		Orientation        orientation;
	};

	struct ExecutedPathHashEquals
	{
		bool operator()(const ExecutedPath& path) const
		{
			size_t seed = 0;
			boost::hash_combine(seed, path.map.hashCode());
			boost::hash_combine(seed, hashCode(path.position));
			boost::hash_combine(seed, path.orientation);
			return seed;
		}

		bool operator()(const ExecutedPath& path1, const ExecutedPath& path2) const
		{
			return path1.position == path2.position && path1.orientation == path2.orientation && path1.map == path2.map;
		}
	};

	std::unordered_set<ExecutedPath, ExecutedPathHashEquals, ExecutedPathHashEquals> executeAll(const image::Image<char>& map, const CommandPath& path)
	{
		auto result = std::unordered_set<ExecutedPath, ExecutedPathHashEquals, ExecutedPathHashEquals>{};

		auto pathMap      = image::Image<bool>{ map.getWidth(), map.getHeight(), false };
		auto position     = map.getIJ(std::distance(map.begin(), std::find(map.begin(), map.end(), '^')));
		auto orientation  = Orientation::North;
		pathMap(position) = true;

		result.insert(ExecutedPath{ pathMap, position, orientation });
		for (const auto& command : path) {
			orientation = turned(orientation, command.direction);
			for (size_t i = 0; i < command.steps; ++i) {
				position          = forwarded(position, orientation, 1);
				pathMap(position) = true;
			}
			result.insert(ExecutedPath{ pathMap, position, orientation });
		}

		return result;
	}

	ExecutedPath execute(const image::Image<char>& map, const CommandPath& path)
	{
		auto pathMap = image::Image<bool>{ map.getWidth(), map.getHeight(), false };

		auto position    = map.getIJ(std::distance(map.begin(), std::find(map.begin(), map.end(), '^')));
		auto orientation = Orientation::North;

		pathMap(position) = true;
		for (const auto& command : path) {
			orientation = turned(orientation, command.direction);
			for (size_t i = 0; i < command.steps; ++i) {
				position          = forwarded(position, orientation, 1);
				pathMap(position) = true;
			}
		}

		return { pathMap, position, orientation };
	}

	template<typename Function> void forEachPath(const std::vector<std::int64_t>& code, Function function)
	{
		const auto map         = toMap(toString(opcode::run(code)));
		const auto endPosition = getEndPosition(map);

		auto stack = std::vector<CommandPath>{};
		stack.emplace_back();

		while (!stack.empty()) {
			const auto currentPath = stack.back();
			stack.pop_back();

			const auto executedPath  = execute(map, currentPath);
			const auto executedPaths = executeAll(map, currentPath);

			// std::cout << "\n\n";
			// display(executedPath.map);

			if (executedPath.position == endPosition) {
				function(currentPath);
				continue;
			}

			for (const auto direction : { Command::Left, Command::Right }) {
				const auto orientation = turned(executedPath.orientation, direction);
				auto       position    = forwarded(executedPath.position, orientation, 1);

				if (map.isValid(position) && map(position) == '#') {
					size_t steps = 0;
					while (map.isValid(position) && map(position) == '#') {
						position = forwarded(position, orientation, 1);
						++steps;
					}

					for (size_t i = 1; i < steps; ++i) {
						if (isIntersection(map, forwarded(executedPath.position, orientation, i))) {
							auto nextPath = currentPath;
							nextPath.push_back(Command{ direction, i });

							if (executedPaths.count(execute(map, nextPath)) == 0)
								stack.push_back(nextPath);
						}
					}

					auto nextPath = currentPath;
					nextPath.push_back(Command{ direction, steps });
					if (executedPaths.count(execute(map, nextPath)) == 0)
						stack.push_back(nextPath);
				}
			}
		}
	}

	struct SplitCommand
	{
		std::vector<char> main;
		CommandPath       a, b, c;
	};

	std::string toString(const std::vector<char>& v)
	{
		std::ostringstream out;
		out << v.front();
		for (size_t i = 1; i < v.size(); ++i)
			out << ',' << v[i];
		return out.str();
	}

	std::string toString(const SplitCommand& command)
	{
		const auto main = toString(command.main) + "\n";
		const auto a    = toString(command.a) + "\n";
		const auto b    = toString(command.b) + "\n";
		const auto c    = toString(command.c) + "\n";
		const auto feed = std::string{ "n\n" };
		return main + a + b + c + feed;
	}

	boost::optional<SplitCommand> split(const CommandPath& path, size_t iA, size_t sizeA, size_t iB, size_t sizeB, size_t iC, size_t sizeC)
	{
		const auto firstA = path.begin() + iA;
		const auto lastA  = path.begin() + iA + sizeA;
		const auto firstB = path.begin() + iB;
		const auto lastB  = path.begin() + iB + sizeB;
		const auto firstC = path.begin() + iC;
		const auto lastC  = path.begin() + iC + sizeC;

		auto splitCommand = SplitCommand{};

		auto it = path.begin();
		while (it != path.end()) {

			if (sizeA <= distance(it, path.end()) && std::equal(firstA, lastA, it)) {
				splitCommand.main.push_back('A');
				it += sizeA;
			}
			else if (sizeB <= distance(it, path.end()) && std::equal(firstB, lastB, it)) {
				splitCommand.main.push_back('B');
				it += sizeB;
			}
			else if (sizeC <= distance(it, path.end()) && std::equal(firstC, lastC, it)) {
				splitCommand.main.push_back('C');
				it += sizeC;
			}
			else {
				return boost::none;
			}
		}

		splitCommand.a.insert(splitCommand.a.begin(), firstA, lastA);
		splitCommand.b.insert(splitCommand.b.begin(), firstB, lastB);
		splitCommand.c.insert(splitCommand.c.begin(), firstC, lastC);

		return splitCommand;
	}

	boost::optional<SplitCommand> split(const CommandPath& path)
	{
		for (size_t iA = 0; iA < path.size(); ++iA)
			for (size_t sizeA = 1; sizeA < path.size() - iA - 2; ++sizeA)
				for (size_t iB = iA + sizeA; iB < path.size(); ++iB)
					for (size_t sizeB = 1; sizeB < path.size() - iB - 1; ++sizeB)
						for (size_t iC = iB + sizeB; iC < path.size(); ++iC)
							for (size_t sizeC = 1; sizeC < path.size() - iC; ++sizeC) {
								auto splitCommand = split(path, iA, sizeA, iB, sizeB, iC, sizeC);
								if (splitCommand)
									return splitCommand;
							}
		return boost::none;
	}

	struct FoundException : std::exception
	{
		FoundException(SplitCommand command) : command{ std::move(command) } {}
		SplitCommand command;
	};

	void runPart1(const std::vector<std::int64_t>& code)
	{
		const auto map = toMap(toString(opcode::run(code)));
		std::cout << "Part 1: " << getSumOfAlignments(map) << "\n";
	}

	void runPart2(std::vector<std::int64_t> code)
	{
		try {
			forEachPath(code, [](const CommandPath& path) {
				const auto splitCommand = split(path);
				if (splitCommand)
					throw FoundException{ *splitCommand };
			});
		}
		catch (const FoundException& e) {

			code[0] = 2;

			const auto command = toString(e.command);
			const auto inputs  = toIntegers(command);
			const auto outputs = opcode::run(code, inputs);
			std::cout << "Part 2: " << outputs.back() << "\n";
		}
	}
}

int main(int argc, char* argv[])
{
	const auto code = io::readLineOfIntegers("day17_input.txt");
	runPart1(code);
	runPart2(code);
	std::cin.get();
}