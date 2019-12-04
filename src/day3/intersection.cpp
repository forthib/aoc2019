#include "intersection.h"

namespace intersection
{
	std::vector<Point> path(const std::vector<std::string>& wire)
	{
		auto result = std::vector<Point>{};
		auto point = Point{ 0, 0 };
		result.push_back(point);
		for (const auto& instruction : wire)
		{
			const auto valueStr = instruction.substr(1);
			const size_t value = std::atoi(valueStr.c_str());
			switch (instruction[0])
			{
			case 'R': point[0] += value; break;
			case 'L': point[0] -= value; break;
			case 'U': point[1] += value; break;
			case 'D': point[1] -= value; break;
			default: throw std::exception{ "unsupported instruction" };
			}
			result.push_back(point);
		}
		return result;
	}

	namespace
	{
		bool intersectionHV(Point A, Point B, Point C, Point D, Point& I)
		{
			if (A[0] == B[0] && C[1] == D[1])
			{
				const bool interX = A[0] >= C[0] && A[0] <= D[0] || A[0] >= D[0] && A[0] <= C[0];
				const bool interY = C[1] >= A[1] && C[1] <= B[1] || C[1] >= B[1] && C[1] <= A[1];
				if (interX && interY)
				{
					I[0] = A[0];
					I[1] = C[1];
					return true;
				}
			}
			return false;
		}
	}

	bool intersection(Point A, Point B, Point C, Point D, Point& I)
	{
		return intersectionHV(A, B, C, D, I) || intersectionHV(C, D, A, B, I);
	}

	size_t distance(const Point& point1, const Point& point2)
	{
		return std::abs(point1[0] - point2[0]) + std::abs(point1[1] - point2[1]);
	}

	size_t distance(const std::vector<Point>& path1, const std::vector<Point>& path2)
	{
		const auto origin = Point{ 0, 0 };
		size_t result = std::numeric_limits<size_t>::max();

		for (size_t i = 1; i < path1.size(); ++i)
		{
			for (size_t j = 1; j < path2.size(); ++j)
			{
				auto inter = Point{};
				if (intersection(path1[i - 1], path1[i], path2[j - 1], path2[j], inter) && distance(origin, inter) > 0 && distance(origin, inter) < result)
					result = distance(origin, inter);
			}
		}

		return result;
	}

	size_t distance(const std::vector<std::string>& wire1, const std::vector<std::string>& wire2)
	{
		return distance(path(wire1), path(wire2));
	}

	size_t steps(const std::vector<Point>& path, size_t first, size_t last)
	{
		size_t result = 0;
		for (size_t i = first + 1; i < last; ++i)
			result += distance(path[i - 1], path[i]);
		return result;
	}

	size_t steps(const std::vector<Point>& path1, const std::vector<Point>& path2)
	{
		const auto origin = Point{ 0, 0 };
		size_t result = std::numeric_limits<size_t>::max();

		for (size_t i = 1; i < path1.size(); ++i)
		{
			for (size_t j = 1; j < path2.size(); ++j)
			{
				auto inter = Point{};
				if (intersection(path1[i - 1], path1[i], path2[j - 1], path2[j], inter) && distance(origin, inter) > 0)
				{
					const size_t steps1 = steps(path1, 0, i) + distance(path1[i - 1], inter);
					const size_t steps2 = steps(path2, 0, j) + distance(path2[j - 1], inter);
					if (steps1 + steps2 < result)
						result = steps1 + steps2;
				}
			}
		}

		return result;
	}

	size_t steps(const std::vector<std::string>& wire1, const std::vector<std::string>& wire2)
	{
		return steps(path(wire1), path(wire2));
	}
}