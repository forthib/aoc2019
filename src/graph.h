#pragma once

#include "image.h"
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <iostream>
#include <vector>

namespace graph {

	using Edges = std::vector<std::vector<std::pair<size_t, size_t>>>;

	template<typename V> struct Graph
	{
		std::vector<V>                vertices;
		std::unordered_map<V, size_t> vertexToIndex;
		image::Image<size_t>          edges{ 0, 0 };
	};

	template<typename V> size_t getVertexIndex(const Graph<V>& g, const V& v) { return g.vertexToIndex.at(v); }

	template<typename V> size_t getEdgeWeight(const Graph<V>& g, const V& v1, const V& v2)
	{
		const auto iV1 = getVertexIndex(g, v1);
		const auto iV2 = getVertexIndex(g, v2);
		return g.edges(iV1, iV2);
	}

	template<typename V> size_t hasEdge(const Graph<V>& g, const V& v1, const V& v2) { return getEdgeWeight(g, v1, v2) != std::numeric_limits<size_t>::max(); }

	template<typename V> void resize(Graph<V>& g, size_t n)
	{
		g.vertices.resize(n);
		resize(g.edges, n, n, std::numeric_limits<size_t>::max());
	}

	template<typename V> size_t addVertex(Graph<V>& g, V v)
	{
		const size_t index = g.vertices.size();
		g.vertices.push_back(std::move(v));
		g.vertexToIndex[v] = index;
		resize(g.edges, index + 1, index + 1, std::numeric_limits<size_t>::max());
		return index;
	}

	template<typename V> void setVertex(Graph<V>& g, size_t iV, V v)
	{
		g.vertices[iV]     = std::move(v);
		g.vertexToIndex[v] = iV;
	}

	template<typename V> void addEdge(Graph<V>& g, const V& v1, const V& v2, size_t weight = 1)
	{
		const auto iV1    = getVertexIndex(g, v1);
		const auto iV2    = getVertexIndex(g, v2);
		g.edges(iV1, iV2) = weight;
	}

	template<typename V, typename VDisplay> void display(const Graph<V>& g, std::ostream& out, VDisplay vDisplay)
	{
		for (size_t iV1 = 0; iV1 < g.vertices.size(); ++iV1) {
			vDisplay(out, g.vertices[iV1]) << " ->";
			for (size_t iV2 = 0; iV2 < g.vertices.size(); ++iV2)
				if (g.edges(iV1, iV2) != std::numeric_limits<size_t>::max())
					vDisplay(out << " ", g.vertices[iV2]) << ":" << g.edges(iV1, iV2);
			out << "\n";
		}
	}

	template<typename V> void display(const Graph<V>& g, std::ostream& out = std::cout)
	{
		return display(g, out, [](auto& out, const auto& v) -> std::ostream& { return out << v; });
	}

	template<typename U, typename V, typename Function> Graph<U> transform(const Graph<V>& graph, Function function)
	{
		auto result = Graph<U>{};
		for (const auto& v : graph.vertices)
			result.vertices.push_back(function(v));
		for (auto&& entry : graph.vertexToIndex)
			result.vertexToIndex[function(entry.first)] = entry.second;
		result.edges = graph.edges;
		return result;
	}

	template<typename V> struct Exploration
	{
		std::vector<V>                vertices;
		std::unordered_map<V, size_t> vertexToIndex;
		std::vector<size_t>           distances;
		std::vector<size_t>           predecessors;
	};

	template<typename V> std::vector<Exploration<V>> explore(const Graph<V>& g, const std::vector<V>& froms)
	{
		using BWeightProperty = boost::property<boost::edge_weight_t, size_t>;
		using BNameProperty   = boost::property<boost::vertex_name_t, size_t>;
		using BGraph          = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, BNameProperty, BWeightProperty>;

		BGraph bg;
		for (size_t iV = 0; iV < g.vertices.size(); ++iV)
			add_vertex(iV, bg);
		for (size_t iV1 = 0; iV1 < g.vertices.size(); ++iV1)
			for (size_t iV2 = 0; iV2 < g.vertices.size(); ++iV2)
				if (g.edges(iV1, iV2) != std::numeric_limits<size_t>::max())
					add_edge(iV1, iV2, g.edges(iV1, iV2), bg);

		auto result = std::vector<Exploration<V>>{};
		for (const auto& from : froms) {
			auto exploration          = Exploration<V>{};
			exploration.vertices      = g.vertices;
			exploration.vertexToIndex = g.vertexToIndex;
			exploration.distances.resize(g.vertices.size());
			exploration.predecessors.resize(g.vertices.size());
			dijkstra_shortest_paths(
			    bg, getVertexIndex(g, from), boost::predecessor_map(exploration.predecessors.data()).distance_map(exploration.distances.data()));
			result.push_back(exploration);
		}
		return result;
	}

	template<typename V> Exploration<V> explore(const Graph<V>& g, const V& from) { return explore(g, std::vector<V>{ from }).front(); }

	template<typename V> size_t getVertexIndex(const Exploration<V>& e, const V& v) { return e.vertexToIndex.at(v); }

	template<typename V> size_t getDistance(const Exploration<V>& e, const V& to)
	{
		const size_t index = getVertexIndex(e, to);
		return e.distances[index];
	}

	template<typename V> size_t isReachable(const Exploration<V>& e, const V& to)
	{ //
		return getDistance(e, to) != std::numeric_limits<size_t>::max();
	}

	template<typename V> std::vector<V> getReachableVertices(const Exploration<V>& e)
	{
		auto vertices = std::vector<V>{};
		for (size_t i = 0; i < e.vertices.size(); ++i)
			if (e.distances[i] != std::numeric_limits<size_t>::max())
				vertices.push_back(e.vertices[i]);
		return vertices;
	}

	template<typename V> std::vector<V> getPath(const Exploration<V>& e, const V& to)
	{
		auto path = std::vector<V>{};
		path.push_back(to);

		size_t index = getVertexIndex(e, to);
		while (e.predecessors[index] != index) {
			index = e.predecessors[index];
			path.push_back(e.vertices[index]);
		}

		if (e.distances[index] > 0)
			return {};

		std::reverse(path.begin(), path.end());
		return path;
	}

	template<typename V> bool isOnPath(const Exploration<V>& e, const V& v, const V& to)
	{
		const size_t i1 = getVertexIndex(e, v);
		size_t       i2 = getVertexIndex(e, to);
		while (e.predecessors[i2] != i2 && i1 != i2)
			i2 = e.predecessors[i2];
		return i1 == i2;
	}

	template<typename V> auto getDistance(const Graph<V>& g, const V& from, const V& to) { return getDistance(explore(g, from), to); }
	template<typename V> auto isReachable(const Graph<V>& g, const V& from, const V& to) { return isReachable(explore(g, from), to); }
	template<typename V> auto getReachableVertices(const Graph<V>& g, const V& from) { return getReachableVertices(explore(g, from)); }
	template<typename V> auto getPath(const Graph<V>& g, const V& from, const V& to) { return getPath(explore(g, from), to); }
	template<typename V> auto isOnPath(const Graph<V>& g, const V& from, const V& v, const V& to) { return isOnPath(explore(g, from), v, to); }
}
