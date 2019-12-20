#include "graph.h"
#include "image.h"
#include "io.h"
#include "test.h"
#include <numeric>

using namespace image;
using namespace graph;

namespace {

	Image<char> readRawMap(const std::string& fileName)
	{
		const auto   lines  = io::readLinesOfChars(fileName);
		const size_t height = lines.size();
		const size_t width  = lines.front().size();

		auto map = Image<char>{ width, height };
		for (size_t j = 0; j < height; ++j)
			for (size_t i = 0; i < width; ++i)
				map(i, j) = lines[j][i];
		return map;
	}

	size_t countSpacesInRow(const Image<char>& rawMap, size_t jRow)
	{
		size_t cpt = 0;
		for (size_t i = 0; i < rawMap.getWidth(); ++i)
			if (rawMap(i, jRow) == ' ')
				cpt++;
		return cpt;
	}

	size_t countSpacesInColumn(const Image<char>& rawMap, size_t iCol)
	{
		size_t cpt = 0;
		for (size_t j = 0; j < rawMap.getHeight(); ++j)
			if (rawMap(iCol, j) == ' ')
				cpt++;
		return cpt;
	}

	struct Box
	{
		size_t minI, maxI, minJ, maxJ;
	};

	Box findHole(const Image<char>& rawMap)
	{
		size_t minI = rawMap.getWidth() - 3;
		size_t maxI = 2;
		size_t minJ = rawMap.getHeight() - 3;
		size_t maxJ = 2;

		for (size_t i = 2; i < rawMap.getWidth() - 2; ++i) {
			if (countSpacesInColumn(rawMap, i) > 4) {
				minI = std::min(minI, i);
				maxI = std::max(maxI, i);
			}
		}

		for (size_t j = 2; j < rawMap.getHeight() - 2; ++j) {
			if (countSpacesInRow(rawMap, j) > 4) {
				minJ = std::min(minJ, j);
				maxJ = std::max(maxJ, j);
			}
		}

		return { minI, maxI, minJ, maxJ };
	}

	bool contains(const Box& box, size_t i, size_t j) { return i >= box.minI && i <= box.maxI && j >= box.minJ && j <= box.maxJ; }
	bool onBorder(const Box& box, size_t i, size_t j) { return contains(box, i, j) && (i == box.minI || i == box.maxI || j == box.minJ || j == box.maxJ); }

	bool isLeftBorder(const Box& extBox, const Box& intBox, size_t i, size_t j)
	{
		return (contains(extBox, i, j) && i == extBox.minI) || (contains(intBox, i, j) && i == intBox.maxI);
	}

	bool isRightBorder(const Box& extBox, const Box& intBox, size_t i, size_t j)
	{
		return (contains(extBox, i, j) && i == extBox.maxI) || (contains(intBox, i, j) && i == intBox.minI);
	}

	bool isTopBorder(const Box& extBox, const Box& intBox, size_t i, size_t j)
	{
		return (contains(extBox, i, j) && j == extBox.minJ) || (contains(intBox, i, j) && j == intBox.maxJ);
	}

	bool isBottomBorder(const Box& extBox, const Box& intBox, size_t i, size_t j)
	{
		return (contains(extBox, i, j) && j == extBox.maxJ) || (contains(intBox, i, j) && j == intBox.minJ);
	}

	constexpr int64_t free = 0;
	constexpr int64_t wall = -1;

	int64_t getPortalIndex(char c1, char c2, bool outer = true)
	{
		if (c1 == 'A' && c2 == 'A' || c1 == 'Z' && c2 == 'Z')
			outer = true;
		const auto index = static_cast<int64_t>(c1 - 'A' + 1) * 100 + static_cast<int64_t>(c2 - 'A' + 1);
		return outer ? 2 * index : 2 * index + 1;
	}

	bool isOuterPortalIndex(int64_t index) { return index % 2 == 0; }
	bool isInnerPortalIndex(int64_t index) { return index % 2 == 1; }

	int64_t getPortalIndex(const std::string& name, bool outer = true) { return getPortalIndex(name[0], name[1], outer); }

	int64_t getDualPortalIndex(int64_t index)
	{
		if (index % 2 == 0)
			return index + 1;
		else
			return index - 1;
	}

	Image<int64_t> readMap(const std::string& fileName)
	{
		const auto rawMap = readRawMap(fileName);
		const auto hole   = findHole(rawMap);
		const auto extBox = Box{ 2, rawMap.getWidth() - 3, 2, rawMap.getHeight() - 3 };
		const auto intBox = Box{ hole.minI - 1, hole.maxI + 1, hole.minJ - 1, hole.maxJ + 1 };

		const size_t width  = rawMap.getWidth() - 4;
		const size_t height = rawMap.getHeight() - 4;

		auto map = Image<int64_t>{ width, height, wall };
		for (size_t j = 0; j < height; ++j) {
			for (size_t i = 0; i < width; ++i) {
				const auto x = i + 2;
				const auto y = j + 2;

				if (contains(hole, x, y))
					continue;

				if (rawMap(x, y) == '.') {
					map(i, j) = free;

					const bool outer = onBorder(extBox, x, y);

					if (isLeftBorder(extBox, intBox, x, y))
						map(i, j) = getPortalIndex(rawMap(x - 2, y), rawMap(x - 1, y), outer);
					else if (isRightBorder(extBox, intBox, x, y))
						map(i, j) = getPortalIndex(rawMap(x + 1, y), rawMap(x + 2, y), outer);
					else if (isTopBorder(extBox, intBox, x, y))
						map(i, j) = getPortalIndex(rawMap(x, y - 2), rawMap(x, y - 1), outer);
					else if (isBottomBorder(extBox, intBox, x, y))
						map(i, j) = getPortalIndex(rawMap(x, y + 1), rawMap(x, y + 2), outer);
				}
			}
		}
		return map;
	}

	Image<char> pretty(const Image<int64_t>& map)
	{
		return image::transform<char>(map, [](const auto value) {
			if (value == wall)
				return '#';
			if (value == free)
				return '.';
			if (value == getPortalIndex('A', 'A'))
				return 'A';
			if (value == getPortalIndex('Z', 'Z'))
				return 'Z';
			return 'X';
		});
	}

	Graph<IJ> makeGraph(const Image<int64_t>& map, bool withTeleports = true)
	{
		auto vertices = std::vector<IJ>{};
		for (size_t index = 0; index < map.size(); ++index)
			if (map(index) != wall)
				vertices.push_back(map.getIJ(index));

		auto g = Graph<IJ>{};
		resize(g, vertices.size());

		for (size_t index = 0; index < vertices.size(); ++index)
			setVertex(g, index, vertices[index]);

		for (size_t j = 0; j < map.getHeight(); ++j) {
			for (size_t i = 0; i < map.getWidth(); ++i) {
				if (map(i, j) != wall) {
					if (i > 0 && map(i - 1, j) != wall)
						addEdge(g, { i, j }, { i - 1, j });
					if (i < map.getWidth() - 1 && map(i + 1, j) != wall)
						addEdge(g, { i, j }, { i + 1, j });
					if (j > 0 && map(i, j - 1) != wall)
						addEdge(g, { i, j }, { i, j - 1 });
					if (j < map.getHeight() - 1 && map(i, j + 1) != wall)
						addEdge(g, { i, j }, { i, j + 1 });
				}
			}
		}

		if (withTeleports) {
			for (size_t index1 = 0; index1 < map.size(); ++index1) {
				if (map(index1) <= 0)
					continue;
				const size_t index2 = map.findIndex(getDualPortalIndex(map(index1)));
				if (index2 == map.size())
					continue;
				const auto p1 = map.getIJ(index1);
				const auto p2 = map.getIJ(index2);
				addEdge(g, p1, p2);
			}
		}

		return g;
	}

	Graph<int64_t> simplify(const Image<int64_t>& map, const Graph<IJ>& graph)
	{
		auto vertices = std::vector<IJ>{};
		for (size_t index = 0; index < map.size(); ++index)
			if (map(index) > 0)
				vertices.push_back(map.getIJ(index));

		const auto explorations = explore(graph, vertices);

		auto result = Graph<int64_t>{};
		resize(result, vertices.size());

		for (size_t index = 0; index < vertices.size(); ++index)
			setVertex(result, index, map(vertices[index]));

		for (size_t index1 = 0; index1 < vertices.size(); ++index1) {
			const auto& exploration = explorations[index1];
			for (size_t index2 = index1 + 1; index2 < vertices.size(); ++index2) {
				const auto p2 = vertices[index2];
				if (isReachable(exploration, p2)) {
					const auto distance          = getDistance(exploration, p2);
					result.edges(index1, index2) = distance;
					result.edges(index2, index1) = distance;
				}
			}
		}

		return result;
	}

	Graph<int64_t> removeAAZZ(const Graph<int64_t>& graph)
	{
		const auto valueAA = getPortalIndex("AA");
		const auto valueZZ = getPortalIndex("ZZ");

		auto result = Graph<int64_t>{};

		for (const auto v : graph.vertices)
			if (v != valueAA && v != valueZZ)
				addVertex(result, v);

		for (const auto v1 : graph.vertices) {
			if (v1 == valueAA || v1 == valueZZ)
				continue;
			const size_t indexV1 = getVertexIndex(graph, v1);
			for (const auto v2 : graph.vertices) {
				if (v2 == valueAA || v2 == valueZZ)
					continue;
				const size_t indexV2 = getVertexIndex(graph, v2);
				const auto   weight  = graph.edges(indexV1, indexV2);
				addEdge(result, v1, v2, weight);
			}
		}

		return result;
	}

	Graph<int64_t> removeNegativeVertices(const Graph<int64_t>& graph)
	{
		auto result = Graph<int64_t>{};

		for (const auto& v : graph.vertices)
			if (v >= 0)
				addVertex(result, v);

		const auto explorations = explore(graph, result.vertices);
		for (const auto& v1 : result.vertices)
			for (const auto& v2 : result.vertices)
				if (v1 != v2)
					addEdge(result, v1, v2, getDistance(explorations[result.vertexToIndex.at(v1)], v2));

		return result;
	}

	Graph<std::string> pretty(const Graph<int64_t>& graph)
	{
		return transform<std::string>(graph, [&](const auto& v) {
			const auto index  = std::abs(v / 2);
			const auto c1     = static_cast<char>('A' + index / 100 - 1);
			const auto c2     = static_cast<char>('A' + index % 100 - 1);
			const auto prefix = v < 0 ? "-" : "";
			const auto suffix = v % 2 == 0 ? 'o' : 'i';
			return std::string{} + prefix + c1 + c2 + suffix;
		});
	}

	Graph<int64_t> cumulate(const Graph<int64_t>& outerGraph, const Graph<int64_t>& innerGraph)
	{
		auto result = Graph<int64_t>{};

		const auto outer = [](const auto v) { return isOuterPortalIndex(v) ? v : -v; };
		const auto inner = [](const auto v) { return isInnerPortalIndex(v) ? v : -v; };

		for (const auto& v : outerGraph.vertices)
			addVertex(result, outer(v));

		for (const auto& v : innerGraph.vertices)
			addVertex(result, inner(v));

		for (const auto& v1 : outerGraph.vertices)
			for (const auto& v2 : outerGraph.vertices)
				addEdge(result, outer(v1), outer(v2), getEdgeWeight(outerGraph, v1, v2));

		for (const auto& v1 : innerGraph.vertices)
			for (const auto& v2 : innerGraph.vertices)
				addEdge(result, inner(v1), inner(v2), getEdgeWeight(innerGraph, v1, v2));

		for (const auto& v1 : outerGraph.vertices)
			for (const auto& v2 : innerGraph.vertices)
				if (isInnerPortalIndex(v1) && isOuterPortalIndex(v2) && v1 == getDualPortalIndex(v2)) {
					addEdge(result, outer(v1), inner(v2), 1);
					addEdge(result, inner(v2), outer(v1), 1);
				}

		return removeNegativeVertices(result);
	}

	bool isBetterConnected(const Graph<int64_t>& g1, const Graph<int64_t>& g2)
	{
		for (const auto& v1 : g1.vertices)
			for (const auto& v2 : g1.vertices)
				if (getEdgeWeight(g1, v1, v2) < getEdgeWeight(g2, v1, v2))
					return true;
		return false;
	}

	size_t countSteps(const Graph<int64_t>& graph)
	{ //
		return getDistance(graph, getPortalIndex("AA"), getPortalIndex("ZZ"));
	}

	size_t countStepsPart1(const std::string& fileName)
	{
		const auto map   = readMap(fileName);
		const auto graph = simplify(map, makeGraph(map));
		return countSteps(graph);
	}

	size_t countStepsPart2(const std::string& fileName)
	{
		const auto map        = readMap(fileName);
		const auto outerGraph = simplify(map, makeGraph(map, false));
		const auto innerGraph = removeAAZZ(outerGraph);

		auto graph = outerGraph;
		for (size_t i = 1;; ++i) {
			const auto previousGraph = graph;
			graph                    = cumulate(graph, innerGraph);

			if (!isBetterConnected(graph, previousGraph))
				break;

			const auto steps = countSteps(graph);
			if (steps != std::numeric_limits<size_t>::max())
				return steps;
		}
		return std::numeric_limits<size_t>::max();
	}
}

int main(int argc, char* argv[])
{
	test::equals(countStepsPart1("day20_test1.txt"), 23);
	test::equals(countStepsPart1("day20_test2.txt"), 58);
	test::equals(countStepsPart2("day20_test2.txt"), std::numeric_limits<size_t>::max());
	test::equals(countStepsPart2("day20_test3.txt"), 396);

	std::cout << "Part 1: " << countStepsPart1("day20_input.txt") << "\n";
	std::cout << "Part 2: " << countStepsPart2("day20_input.txt") << "\n";

	std::cin.get();
}