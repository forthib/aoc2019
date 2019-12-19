#include "graph.h"
#include "image.h"
#include "io.h"
#include "opcode.h"
#include "test.h"
#include <numeric>

using namespace image;
using namespace graph;

namespace {

	Image<char> toMap(const std::vector<std::vector<char>>& input)
	{
		const size_t height = input.size();
		const size_t width  = input.front().size();
		auto         map    = Image<char>{ width, height };
		for (size_t j = 0; j < height; ++j)
			for (size_t i = 0; i < width; ++i)
				map(i, j) = input[j][i];
		return map;
	}

	bool isEmpty(char c) { return c == '.' || c == '@'; }
	bool isWall(char c) { return c == '#'; }
	bool isDoor(char c) { return (c >= 'A' && c <= 'Z'); }
	bool isKey(char c) { return (c >= 'a' && c <= 'z'); }
	bool isElement(char c) { return isKey(c) || isDoor(c); }
	bool isBlocked(char c) { return isWall(c) || isDoor(c); }
	bool isFree(char c) { return isEmpty(c) || isKey(c); }
	char toDoor(char key) { return static_cast<char>(key - 'a' + 'A'); }
	char toKey(char door) { return static_cast<char>(door - 'A' + 'a'); }

	std::vector<char> getAllKeys(const Image<char>& map)
	{
		auto keys = std::vector<char>{};
		for (size_t j = 0; j < map.getHeight(); ++j)
			for (size_t i = 0; i < map.getWidth(); ++i)
				if (isKey(map(i, j)))
					keys.push_back(map(i, j));
		std::sort(keys.begin(), keys.end());
		return keys;
	}

	std::vector<IJ> getAllKeyPositions(const Image<char>& map)
	{
		auto result = std::vector<IJ>{};
		for (const auto& key : getAllKeys(map))
			result.push_back(map.findIJ(key));
		return result;
	}

	Graph<IJ> makeGraph(const Image<char>& map, bool useDoors = true)
	{
		auto vertices      = std::vector<IJ>{};
		auto vertexToIndex = std::unordered_map<IJ, size_t>{};
		for (size_t index = 0; index < map.size(); ++index)
			if (!isWall(map(index))) {
				vertexToIndex[map.getIJ(index)] = vertices.size();
				vertices.push_back(map.getIJ(index));
			}

		auto g = Graph<IJ>{};
		resize(g, vertices.size());

		for (size_t index = 0; index < vertices.size(); ++index)
			setVertex(g, index, vertices[index]);

		for (size_t j = 0; j < map.getHeight(); ++j)
			for (size_t i = 0; i < map.getWidth(); ++i)
				if (useDoors ? isFree(map(i, j)) : !isWall(map(i, j))) {
					if (i > 0 && !isWall(map(i - 1, j)))
						g.edges(vertexToIndex.at({ i, j }), vertexToIndex.at({ i - 1, j })) = 1;
					if (i < map.getWidth() - 1 && !isWall(map(i + 1, j)))
						g.edges(vertexToIndex.at({ i, j }), vertexToIndex.at({ i + 1, j })) = 1;
					if (j > 0 && !isWall(map(i, j - 1)))
						g.edges(vertexToIndex.at({ i, j }), vertexToIndex.at({ i, j - 1 })) = 1;
					if (j < map.getHeight() - 1 && !isWall(map(i, j + 1)))
						g.edges(vertexToIndex.at({ i, j }), vertexToIndex.at({ i, j + 1 })) = 1;
				}

		return g;
	}

	template<typename K, typename V, typename Predicate> std::unordered_map<K, V> filter(const std::unordered_map<K, V>& map, Predicate pred)
	{
		auto result = std::unordered_map<K, V>{};
		for (auto&& entry : map)
			if (pred(entry))
				result.insert(entry);
		return result;
	}

	Graph<IJ> makeDistanceGraph(const Image<char>& map, const Graph<IJ>& mapGraph)
	{
		const auto startPositions = map.findIJs('@');
		const auto keyPositions   = getAllKeyPositions(map);

		auto allPositions = keyPositions;
		allPositions.insert(allPositions.end(), startPositions.begin(), startPositions.end());

		auto graph = Graph<IJ>{};
		for (const auto& p : allPositions)
			addVertex(graph, p);
		for (const auto& p1 : allPositions) {
			const auto exploration = explore(mapGraph, p1);
			for (const auto& p2 : allPositions)
				addEdge(graph, p1, p2, getDistance(exploration, p2));
		}
		return graph;
	}

	Graph<IJ> makePrecedenceGraph(const Image<char>& map, const Graph<IJ>& mapGraph)
	{
		auto graph = Graph<IJ>{};
		for (const auto& startPosition : map.findIJs('@')) {
			const auto keyPositions = getAllKeyPositions(map);
			const auto exploration  = explore(mapGraph, startPosition);

			for (const auto& p : keyPositions)
				addVertex(graph, p);

			for (const auto& p1 : keyPositions) {
				const auto key1 = map(p1);
				const auto path = getPath(exploration, p1);
				for (const auto& p2 : path) {
					if (isDoor(map(p2))) {
						const auto door2    = map(p2);
						const auto key2     = toKey(door2);
						const auto distance = getDistance(mapGraph, p1, map.findIJ(key2));
						addEdge(graph, map.findIJ(key2), p1, distance);
					}
					else if (isKey(map(p2))) {
						const auto key2 = map(p2);
						if (key1 == key2)
							continue;
						const auto distance = getDistance(exploration, p1) - getDistance(exploration, p2);
						addEdge(graph, p2, p1, distance);
					}
				}
			}
		}
		return graph;
	}

	struct State
	{
		std::vector<size_t>              from;
		std::vector<std::vector<size_t>> indexes;
	};

	struct StateHashEquals
	{
		size_t operator()(const State& s) const noexcept
		{
			size_t seed = 0;
			for (const auto& f : s.from)
				boost::hash_combine(seed, f);
			boost::hash_combine(seed, s.indexes.size());
			for (const auto& i : s.indexes) {
				boost::hash_combine(seed, i.size());
				for (const auto& index : i)
					boost::hash_combine(seed, index);
			}
			return seed;
		}

		size_t operator()(const State& s1, const State& s2) const noexcept { return s1.from == s2.from && s1.indexes == s2.indexes; }
	};

	using StateCache = std::unordered_map<State, std::pair<size_t, std::vector<size_t>>, StateHashEquals, StateHashEquals>;

	size_t countSteps(const Image<char>& map, const Graph<IJ>& distanceGraph, const Graph<IJ>& precedenceGraph, const State& state, StateCache& cache)
	{
		if (std::all_of(state.indexes.begin(), state.indexes.end(), [](const auto& indexes) { return indexes.empty(); })) {
			cache[state] = { 0, {} };
			return 0;
		}

		if (cache.count(state) > 0)
			return cache.at(state).first;

		size_t minSteps     = std::numeric_limits<size_t>::max();
		auto   minStepsPath = std::vector<size_t>{};
		for (size_t iStartPosition = 0; iStartPosition < state.from.size(); ++iStartPosition) {

			const auto  from    = state.from[iStartPosition];
			const auto& indexes = state.indexes[iStartPosition];

			for (size_t iIndex = 0; iIndex < indexes.size(); ++iIndex) {
				const auto nextIndex = indexes[iIndex];

				bool valid = true;
				for (const auto previousIndex : indexes) {
					if (previousIndex != nextIndex && precedenceGraph.edges(previousIndex, nextIndex) != std::numeric_limits<size_t>::max()) {
						valid = false;
						break;
					}
				}

				if (!valid)
					continue;

				auto nextState                 = state;
				nextState.from[iStartPosition] = nextIndex;
				nextState.indexes[iStartPosition].erase(nextState.indexes[iStartPosition].begin() + iIndex);

				const auto stepsNext = distanceGraph.edges(from, nextIndex);
				const auto stepsLeft = countSteps(map, distanceGraph, precedenceGraph, nextState, cache);
				const auto steps     = stepsNext + stepsLeft;

				if (steps < minSteps) {
					minSteps     = steps;
					minStepsPath = cache.at(nextState).second;
					minStepsPath.push_back(nextIndex);
				}
			}
		}

		cache[state] = { minSteps, minStepsPath };

		return minSteps;
	}

	size_t countSteps(const std::string& fileName, bool replace = false)
	{
		auto map = toMap(io::readLinesOfChars(fileName));

		if (replace) {
			const auto ij     = map.findIJ('@');
			const auto i      = ij.i;
			const auto j      = ij.j;
			map(i - 1, j - 1) = '@';
			map(i - 1, j + 1) = '@';
			map(i + 1, j - 1) = '@';
			map(i + 1, j + 1) = '@';
			map(i - 1, j)     = '#';
			map(i + 1, j)     = '#';
			map(i, j - 1)     = '#';
			map(i, j + 1)     = '#';
			map(ij)           = '#';
		}

		const auto startPositions = map.findIJs('@');
		const auto keys           = getAllKeys(map);

		const auto mapGraph        = makeGraph(map, false);
		const auto distanceGraph   = makeDistanceGraph(map, mapGraph);
		const auto precedenceGraph = makePrecedenceGraph(map, mapGraph);

		auto state = State{};
		for (size_t iStartPosition = 0; iStartPosition < startPositions.size(); ++iStartPosition) {
			state.from.push_back(keys.size() + iStartPosition);
			state.indexes.emplace_back();

			for (size_t iKey = 0; iKey < keys.size(); ++iKey)
				if (distanceGraph.edges(iKey, keys.size() + iStartPosition) != std::numeric_limits<size_t>::max())
					state.indexes.back().push_back(iKey);
		}

		auto cache = StateCache{};
		return countSteps(map, distanceGraph, precedenceGraph, state, cache);
	}
}

int main(int argc, char* argv[])
{
	test::equals(countSteps("day18_test1.txt"), 8);
	test::equals(countSteps("day18_test2.txt"), 86);
	test::equals(countSteps("day18_test3.txt"), 132);
	test::equals(countSteps("day18_test4.txt"), 136);
	test::equals(countSteps("day18_test5.txt"), 81);
	test::equals(countSteps("day18_test6.txt"), 8);
	test::equals(countSteps("day18_test7.txt"), 24);
	test::equals(countSteps("day18_test8.txt"), 32);
	test::equals(countSteps("day18_test9.txt"), 70);

	std::cout << "Part 1: " << countSteps("day18_input.txt", false) << "\n";
	std::cout << "Part 2: " << countSteps("day18_input.txt", true) << "\n";

	std::cin.get();
}
