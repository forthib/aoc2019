#include "orbits.h"

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <unordered_map>

namespace orbits
{
	namespace
	{
		using Weight = size_t;
		using WeightProperty = boost::property<boost::edge_weight_t, Weight>;
		using NameProperty = boost::property<boost::vertex_name_t, std::string>;
		using Graph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, NameProperty, WeightProperty>;
		using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
		using IndexMap = boost::property_map<Graph, boost::vertex_index_t>::type;
		using NameMap = boost::property_map<Graph, boost::vertex_name_t>::type;
		using PredecessorMap = boost::iterator_property_map<Vertex*, IndexMap, Vertex, Vertex&>;
		using DistanceMap = boost::iterator_property_map<Weight*, IndexMap, Weight, Weight&>;

		Graph makeGraph(const std::vector<std::array<std::string, 2>>& map, std::unordered_map<std::string, Vertex>& vertices)
		{
			Graph g;

			for (const auto& orbit : map)
				for (const auto& name : orbit)
					if (vertices.count(name) == 0)
						vertices[name] = add_vertex(name, g);

			for (const auto& orbit : map)
			{
				add_edge(vertices.at(orbit[0]), vertices.at(orbit[1]), 1, g);
				add_edge(vertices.at(orbit[1]), vertices.at(orbit[0]), 1, g);
			}

			return g;
		}

		void dijkstra(const Graph& g, const std::unordered_map<std::string, Vertex>& vertices, const std::string& from, std::unordered_map<std::string, size_t>& distances)
		{
			auto predecessorsBuffer = std::vector<Vertex>(num_vertices(g));
			auto distancesBuffer = std::vector<Weight>(num_vertices(g));

			auto indexMap = get(boost::vertex_index, g);
			auto predecessorMap = PredecessorMap(&predecessorsBuffer[0], indexMap);
			auto distanceMap = DistanceMap(&distancesBuffer[0], indexMap);

			dijkstra_shortest_paths(g, vertices.at(from), distance_map(distanceMap).predecessor_map(predecessorMap));

			for (const auto& vertex : vertices)
				distances[vertex.first] = distanceMap[vertex.second];
		}

		const std::string& adjacent(const std::vector<std::array<std::string, 2>>& map, const std::string& name)
		{
			const auto it = std::find_if(map.begin(), map.end(), [&name](const auto& adjacency) { return adjacency[1] == name; });
			return (*it)[0];
		}
	}

	size_t count(const std::vector<std::array<std::string, 2>>& map)
	{
		auto vertices = std::unordered_map<std::string, Vertex>{};
		const auto g = makeGraph(map, vertices);

		auto distances = std::unordered_map<std::string, size_t>{};
		dijkstra(g, vertices, "COM", distances);

		size_t n = 0;
		for (const auto& vertex : vertices)
			n += distances.at(vertex.first);

		return n;
	}

	size_t shortest(const std::vector<std::array<std::string, 2>>& map, const std::string& from, const std::string& to)
	{
		const auto& adjacentFrom = adjacent(map, from);
		const auto& adjacentTo = adjacent(map, to);
		
		auto vertices = std::unordered_map<std::string, Vertex>{};
		const auto g = makeGraph(map, vertices);

		auto distances = std::unordered_map<std::string, size_t>{};
		dijkstra(g, vertices, adjacentFrom, distances);

		return distances.at(adjacentTo);
	}

}