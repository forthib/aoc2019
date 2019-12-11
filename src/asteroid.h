#pragma once

#include "image.h"
#include <iostream>
#include <vector>

namespace asteroid {

	using IJ                       = image::IJ;
	template<typename T> using Map = image::Image<T>;

	Map<char> readMap(const std::string& fileName);
	Map<bool> locateAsteroids(const Map<char>& map);

	size_t getMaxNumberOfDetections(const std::string& fileName);
	size_t getMaxNumberOfDetections(const Map<bool>& map);

	IJ getPositionOfMaxNumberOfDetections(const std::string& fileName);
	IJ getPositionOfMaxNumberOfDetections(const Map<bool>& map);

	Map<size_t> countDetections(const Map<bool>& map);
	size_t      countDetectionsFromLocation(const Map<bool>& map, const IJ& ij);
	Map<bool>   computeDetectionMapFromLocation(const Map<bool>& map, const IJ& ij);

	bool hasAsteroidBetween(const Map<bool>& map, const IJ& ij1, const IJ& ij2);

	std::vector<IJ> destroyAsteroids(const std::string& fileName);
	std::vector<IJ> destroyAsteroids(Map<bool>& map, const IJ& stationIJ);
	std::vector<IJ> destroyAsteroidsOneRotation(Map<bool>& map, const IJ& stationIJ);
}
