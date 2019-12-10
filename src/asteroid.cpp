#include "asteroid.h"

#include "io.h"
#include <algorithm>
#include <sstream>

namespace asteroid {

	namespace {

		template<typename T, typename U, typename Function> Map<T> transformIJ(const Map<U>& inputMap, Function function)
		{
			auto outputMap = Map<T>{ inputMap.getWidth(), inputMap.getHeight() };
			for (size_t j = 0; j < inputMap.getHeight(); ++j)
				for (size_t i = 0; i < inputMap.getWidth(); ++i)
					outputMap(i, j) = function(IJ{ i, j }, inputMap(i, j));
			return outputMap;
		}

		template<typename T, typename U, typename V, typename Function> Map<T> transformIJ(const Map<U>& inputMap1, const Map<V>& inputMap2, Function function)
		{
			if (inputMap1.getWidth() != inputMap2.getWidth() || inputMap1.getHeight() != inputMap2.getHeight())
				throw std::exception{ "input maps have different sizes" };

			auto outputMap = Map<T>{ inputMap1.getWidth(), inputMap1.getHeight() };
			for (size_t j = 0; j < inputMap1.getHeight(); ++j)
				for (size_t i = 0; i < inputMap1.getWidth(); ++i)
					outputMap(i, j) = function(IJ{ i, j }, inputMap1(i, j), inputMap2(i, j));
			return outputMap;
		}

		template<typename T, typename U, typename Function> Map<T> transform(const Map<U>& inputMap, Function function)
		{
			return transformIJ<T>(inputMap, [&](const IJ&, const auto& value) { return function(value); });
		}

		template<typename T, typename U, typename V, typename Function> Map<T> transform(const Map<U>& inputMap1, const Map<V>& inputMap2, Function function)
		{
			return transformIJ<T>(inputMap1, inputMap2, [&](const IJ&, const auto& value1, const auto& value2) { return function(value1, value2); });
		}

		template<typename T>
		void displayMapT(const Map<T>& map, std::ostream& out, const std::string& prefix, const std::string& suffix, const std::string& separator)
		{
			for (size_t j = 0; j < map.getHeight(); ++j) {
				out << prefix;
				out << map(0, j);
				for (size_t i = 1; i < map.getWidth(); ++i)
					out << separator << map(i, j);
				out << suffix;
			}
		}

		template<typename T> T gcd(T x, T y)
		{
			while (y != 0) {
				const auto r = x % y;

				x = y;
				y = r;
			}
			return x;
		}

		double inRange_0_2PI(double theta)
		{
			constexpr double M_PI = 3.14159265358979323846;

			if (theta < 0)
				theta += 2 * M_PI;
			return theta;
		}
	}

	Map<char> readMap(const std::string& fileName)
	{
		const auto   chars  = io::readLinesOfChars(fileName);
		const size_t height = chars.size();
		const size_t width  = chars[0].size();

		auto map = Map<char>{ width, height };
		for (size_t j = 0; j < height; ++j)
			for (size_t i = 0; i < width; ++i)
				map(i, j) = chars[j][i];
		return map;
	}

	Map<bool> locateAsteroids(const Map<char>& map)
	{
		return transform<bool>(map, [](char c) { return c == '#'; });
	}

	size_t getMaxNumberOfDetections(const std::string& fileName) { return getMaxNumberOfDetections(locateAsteroids(readMap(fileName))); }

	size_t getMaxNumberOfDetections(const Map<bool>& map)
	{
		const auto nDetectionsMap = countDetections(map);
		return *std::max_element(nDetectionsMap.begin(), nDetectionsMap.end());
	}

	IJ getPositionOfMaxNumberOfDetections(const std::string& fileName)
	{ //
		return getPositionOfMaxNumberOfDetections(locateAsteroids(readMap(fileName)));
	}

	IJ getPositionOfMaxNumberOfDetections(const Map<bool>& map)
	{
		const auto nDetectionsMap = countDetections(map);
		const auto it             = std::max_element(nDetectionsMap.begin(), nDetectionsMap.end());
		return map.getIJ(std::distance(nDetectionsMap.begin(), it));
	}

	Map<size_t> countDetections(const Map<bool>& map)
	{
		return transformIJ<size_t>(map, [&](const IJ& ij, bool b) { return b ? countDetectionsFromLocation(map, ij) : 0; });
	}

	size_t countDetectionsFromLocation(const Map<bool>& map, const IJ& ij)
	{
		const auto detectionMap = computeDetectionMapFromLocation(map, ij);
		return std::count(detectionMap.begin(), detectionMap.end(), true);
	}

	Map<bool> computeDetectionMapFromLocation(const Map<bool>& map, const IJ& ij)
	{
		auto detectionMap = Map<bool>{ map.getWidth(), map.getHeight(), false };
		for (size_t y = 0; y < map.getHeight(); ++y)
			for (size_t x = 0; x < map.getWidth(); ++x)
				if (x != ij.i || y != ij.j)
					if (map(x, y))
						detectionMap(x, y) = !hasAsteroidBetween(map, ij, { x, y });
		return detectionMap;
	}

	bool hasAsteroidBetween(const Map<bool>& map, const IJ& ij1, const IJ& ij2)
	{
		if (ij1 == ij2)
			return false;

		auto       di = static_cast<std::int64_t>(ij2.i - ij1.i);
		auto       dj = static_cast<std::int64_t>(ij2.j - ij1.j);
		const auto n  = di == 0 ? std::abs(dj) : dj == 0 ? std::abs(di) : gcd(std::abs(di), std::abs(dj));
		di /= n;
		dj /= n;

		for (std::int64_t k = 1; k < n; ++k)
			if (map(ij1.i + k * di, ij1.j + k * dj))
				return true;
		return false;
	}

	std::vector<IJ> destroyAsteroids(const std::string& fileName)
	{
		auto map = locateAsteroids(readMap(fileName));
		return destroyAsteroids(map, getPositionOfMaxNumberOfDetections(map));
	}

	std::vector<IJ> destroyAsteroids(Map<bool>& map, const IJ& stationIndex)
	{
		auto result = std::vector<IJ>{};
		while (std::count(map.begin(), map.end(), true) > 1) {
			const auto rotationResult = destroyAsteroidsOneRotation(map, stationIndex);
			result.insert(result.end(), rotationResult.begin(), rotationResult.end());
		}
		return result;
	}

	std::vector<IJ> destroyAsteroidsOneRotation(Map<bool>& map, const IJ& stationIndex)
	{
		const auto mapAngles = transformIJ<double>(map, [&](const IJ& ij, bool b) {
			if (!b || ij == stationIndex || hasAsteroidBetween(map, ij, stationIndex))
				return std::numeric_limits<double>::quiet_NaN();
			const auto di = static_cast<std::int64_t>(ij.i - stationIndex.i);
			const auto dj = static_cast<std::int64_t>(ij.j - stationIndex.j);
			return inRange_0_2PI(std::atan2(static_cast<double>(di), static_cast<double>(-dj)));
		});

		map = transform<bool>(map, mapAngles, [](bool b, double angle) { return b && isnan(angle); });

		auto indexesAndAngles = std::vector<std::pair<IJ, double>>{};
		for (size_t index = 0; index < mapAngles.size(); ++index)
			if (!std::isnan(mapAngles(index)))
				indexesAndAngles.emplace_back(map.getIJ(index), mapAngles(index));

		std::sort(indexesAndAngles.begin(), indexesAndAngles.end(), [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });

		auto indexes = std::vector<IJ>{};
		for (const auto& indexAndAngle : indexesAndAngles)
			indexes.push_back(indexAndAngle.first);

		return indexes;
	}

	void displayMap(const Map<bool>& map, std::ostream& out, const std::string& prefix, const std::string& suffix)
	{
		return displayMapT(map, out, prefix, suffix, "");
	}

	void displayMap(const Map<char>& map, std::ostream& out, const std::string& prefix, const std::string& suffix)
	{
		return displayMapT(map, out, prefix, suffix, "");
	}

	void displayMap(const Map<size_t>& map, std::ostream& out, const std::string& prefix, const std::string& suffix)
	{
		return displayMapT(map, out, prefix, suffix, "");
	}
}
