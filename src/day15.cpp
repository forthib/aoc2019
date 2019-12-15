#include "image.h"
#include "io.h"
#include "opcode.h"
#include "robot.h"

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

namespace {
	enum class Direction { NORTH = 1, SOUTH = 2, WEST = 3, EAST = 4 };

	std::array<Direction, 4> getAllDirections() { return { Direction::NORTH, Direction::SOUTH, Direction::WEST, Direction::EAST }; }

	robot::Position forwarded(robot::Position position, Direction direction)
	{
		switch (direction) {
		case Direction::NORTH: --position.j; break;
		case Direction::SOUTH: ++position.j; break;
		case Direction::WEST: --position.i; break;
		case Direction::EAST: ++position.i; break;
		default: throw std::exception{ "unsupported direction " };
		}
		return position;
	}

	Direction opposite(Direction direction)
	{
		switch (direction) {
		case Direction::NORTH: return Direction::SOUTH;
		case Direction::SOUTH: return Direction::NORTH;
		case Direction::WEST: return Direction::EAST;
		case Direction::EAST: return Direction::WEST;
		default: throw std::exception{ "unsupported direction " };
		}
	}

	char getValueAt(const std::unordered_map<robot::Position, char>& map, const robot::Position& position)
	{
		if (map.count(position) > 0)
			return map.at(position);
		return ' ';
	}

	struct EndException : std::exception
	{
	};

	using Weight         = size_t;
	using WeightProperty = boost::property<boost::edge_weight_t, Weight>;
	using NameProperty   = boost::property<boost::vertex_name_t, robot::Position>;
	using Graph          = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, NameProperty, WeightProperty>;
	using Vertex         = boost::graph_traits<Graph>::vertex_descriptor;
	using IndexMap       = boost::property_map<Graph, boost::vertex_index_t>::type;
	using NameMap        = boost::property_map<Graph, boost::vertex_name_t>::type;
	using PredecessorMap = boost::iterator_property_map<Vertex*, IndexMap, Vertex, Vertex&>;
	using DistanceMap    = boost::iterator_property_map<Weight*, IndexMap, Weight, Weight&>;

	Graph makeGraph(const std::unordered_map<robot::Position, char>& map, std::unordered_map<robot::Position, Vertex>& vertices)
	{
		Graph g;

		for (auto&& entry : map)
			vertices[entry.first] = add_vertex(entry.first, g);

		for (auto&& entry : map) {
			for (const auto& direction : getAllDirections()) {
				const auto position1 = entry.first;
				const auto position2 = forwarded(position1, direction);
				const auto value1    = getValueAt(map, position1);
				const auto value2    = getValueAt(map, position2);
				if (value1 != ' ' && value1 != '#' && value2 != ' ' && value2 != '#') {
					add_edge(vertices.at(position1), vertices.at(position2), 1, g);
					add_edge(vertices.at(position2), vertices.at(position1), 1, g);
				}
			}
		}

		return g;
	}

	void dijkstra(const Graph& g, const std::unordered_map<robot::Position, Vertex>& vertices, const robot::Position& from,
	    std::unordered_map<robot::Position, size_t>& distances)
	{
		auto predecessorsBuffer = std::vector<Vertex>(num_vertices(g));
		auto distancesBuffer    = std::vector<Weight>(num_vertices(g));

		auto indexMap       = get(boost::vertex_index, g);
		auto predecessorMap = PredecessorMap(&predecessorsBuffer[0], indexMap);
		auto distanceMap    = DistanceMap(&distancesBuffer[0], indexMap);

		dijkstra_shortest_paths(g, vertices.at(from), distance_map(distanceMap).predecessor_map(predecessorMap));

		for (const auto& vertex : vertices)
			distances[vertex.first] = distanceMap[vertex.second];
	}

	std::unordered_map<robot::Position, size_t> getDistancesMap(const std::unordered_map<robot::Position, char>& map, const robot::Position& from)
	{
		auto vertices  = std::unordered_map<robot::Position, Vertex>{};
		auto distances = std::unordered_map<robot::Position, size_t>{};

		const auto g = makeGraph(map, vertices);
		dijkstra(g, vertices, from, distances);

		return distances;
	}

	size_t getNearestPathLength(const std::unordered_map<robot::Position, char>& map)
	{
		const auto from = std::find_if(map.begin(), map.end(), [](auto&& entry) { return entry.second == 'D'; })->first;
		const auto to   = std::find_if(map.begin(), map.end(), [](auto&& entry) { return entry.second == 'A'; })->first;

		return getDistancesMap(map, from).at(to);
	}

	size_t getMaxDistanceFromArrival(const std::unordered_map<robot::Position, char>& map)
	{
		const auto from      = std::find_if(map.begin(), map.end(), [](auto&& entry) { return entry.second == 'A'; })->first;
		const auto distances = getDistancesMap(map, from);

		size_t maxDistance = 0;
		for (auto&& entry : map) {
			if (entry.second == 'D' || entry.second == '.')
				maxDistance = std::max(maxDistance, distances.at(entry.first));
		}
		return maxDistance;
	}
}

int main(int argc, char* argv[])
{
	const auto code = io::readLineOfIntegers("day15_input.txt");

	auto position  = robot::Position{ 0, 0 };
	auto direction = Direction::NORTH;
	auto map       = std::unordered_map<robot::Position, char>{};
	auto path      = std::vector<Direction>{};

	map[position] = 'D';

	const auto inputFunction = [&]() {
		for (const auto nextDirection : getAllDirections()) {
			const auto nextPosition = forwarded(position, nextDirection);
			if (getValueAt(map, nextPosition) == ' ') {
				direction = nextDirection;
				return static_cast<std::int64_t>(direction);
			}
		}

		if (path.empty())
			throw EndException{};

		direction = opposite(path.back());
		return static_cast<std::int64_t>(direction);
	};

	const auto outputFunction = [&](const auto output) {
		switch (output) {
		case 0: {
			map[forwarded(position, direction)] = '#';
			break;
		}
		case 1: {
			position = forwarded(position, direction);
			if (path.empty() || path.back() != opposite(direction)) {
				map[position] = '.';
				path.push_back(direction);
			}
			else
				path.pop_back();
			break;
		}
		case 2: {
			position      = forwarded(position, direction);
			map[position] = 'A';
			path.push_back(direction);
			break;
		}
		default: throw std::exception{ "unsupported output" };
		}
	};

	try {
		opcode::run(code, inputFunction, outputFunction);
	}
	catch (const EndException&) {
	}

	std::cout << "Part 1: " << getNearestPathLength(map) << "\n";
	std::cout << "Part 2: " << getMaxDistanceFromArrival(map) << "\n";

	std::cin.get();
}