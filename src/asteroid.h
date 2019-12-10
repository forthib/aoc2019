#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

namespace asteroid {

	struct IJ
	{
		size_t i = 0, j = 0;
	};

	inline bool operator==(const IJ& lhs, const IJ& rhs) { return lhs.i == rhs.i && lhs.j == rhs.j; }
	inline bool operator!=(const IJ& lhs, const IJ& rhs) { return !operator==(lhs, rhs); }

	template<typename T> class Map
	{
	public:
		Map(size_t width, size_t height, T defaultValue = {}) : width_{ width }, height_{ height }, data_(width * height, defaultValue) {}
		Map(size_t width, size_t height, std::vector<T> data) : width_{ width }, height_{ height }, data_{ std::move(data) } {}

		size_t getWidth() const { return width_; }
		size_t getHeight() const { return height_; }
		size_t size() const { return data_.size(); }

		size_t getIndex(size_t i, size_t j) const { return j * width_ + i; }
		size_t getIndex(const IJ& ij) const { return getIndex(ij.i, ij.j); }
		IJ     getIJ(size_t index) const { return { index % width_, index / width_ }; }

		decltype(auto) operator()(size_t index) const { return data_[index]; }
		decltype(auto) operator()(size_t index) { return data_[index]; }
		decltype(auto) operator()(size_t i, size_t j) const { return data_[getIndex(i, j)]; }
		decltype(auto) operator()(size_t i, size_t j) { return data_[getIndex(i, j)]; }
		decltype(auto) operator()(const IJ& ij) const { return data_[getIndex(ij)]; }
		decltype(auto) operator()(const IJ& ij) { return data_[getIndex(ij)]; }

		decltype(auto) begin() const { return data_.begin(); }
		decltype(auto) begin() { return data_.begin(); }
		decltype(auto) end() const { return data_.end(); }
		decltype(auto) end() { return data_.end(); }

	private:
		size_t         width_, height_;
		std::vector<T> data_;
	};

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

	void displayMap(const Map<bool>& map, std::ostream& out = std::cout, const std::string& prefix = "", const std::string& suffix = "\n");
	void displayMap(const Map<char>& map, std::ostream& out = std::cout, const std::string& prefix = "", const std::string& suffix = "\n");
	void displayMap(const Map<size_t>& map, std::ostream& out = std::cout, const std::string& prefix = "", const std::string& suffix = "\n");
}
