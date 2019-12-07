#pragma once

#include <array>
#include <string>
#include <vector>

namespace intersection
{
	using Point = std::array<intptr_t, 2>;
	
	std::vector<Point> path(const std::vector<std::string>& wire);
	bool intersection(Point A, Point B, Point C, Point D, Point& I);
	
	size_t distance(const Point& point1, const Point& point2);
	size_t distance(const std::vector<Point>& path1, const  std::vector<Point>& path2);
	size_t distance(const std::vector<std::string>& wire1, const std::vector<std::string>& wire2);
	
	size_t steps(const std::vector<Point>& path, size_t first, size_t last);
	size_t steps(const std::vector<Point>& path1, const  std::vector<Point>& path2);
	size_t steps(const std::vector<std::string>& wire1, const std::vector<std::string>& wire2);
}