#pragma once

#include "image.h"
#include <boost/functional/hash.hpp>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace robot {
	enum class Orientation { UP, DOWN, LEFT, RIGHT };

	struct Position
	{
		std::int64_t i = 0, j = 0;
	};

	inline decltype(auto) toTuple(Position& position) { return std::tie(position.i, position.j); }
	inline decltype(auto) toTuple(const Position& position) { return std::tie(position.i, position.j); }

	inline bool operator==(const Position& lhs, const Position& rhs) { return toTuple(lhs) == toTuple(rhs); }
	inline bool operator!=(const Position& lhs, const Position& rhs) { return toTuple(lhs) != toTuple(rhs); }
}

namespace std {
	template<> struct hash<robot::Position>
	{
		size_t operator()(const robot::Position& position) const noexcept
		{
			size_t seed = 0;
			boost::hash_combine(seed, position.i);
			boost::hash_combine(seed, position.j);
			return seed;
		}
	};
}

namespace robot {

	class Robot
	{
	public:
		const Position& getPosition() const { return position_; }

		void decode(std::int64_t value);

	private:
		void turnLeft();
		void turnRight();
		void forward();

		Position    position_;
		Orientation orientation_ = Orientation::UP;
	};

	class PaintingRobot
	{
	public:
		PaintingRobot(std::int64_t firstValue);

		const Position& getPosition() const { return robot_.getPosition(); }

		std::int64_t getCurrentValue() const;
		size_t       getNPaintedPanels() const { return surface_.size(); }

		void decode(std::int64_t value);

		image::Image<std::int64_t> getImage() const;

	private:
		Robot                                      robot_;
		std::unordered_map<Position, std::int64_t> surface_;
		bool                                       painting_ = true;
	};

	struct Box
	{
		std::int64_t minI, maxI, minJ, maxJ;
	};

	template<typename T> Box getBoundingBox(const std::unordered_map<Position, T>& map);

	template<typename T> image::Image<T> toImage(const std::unordered_map<Position, T>& map, const T& defaultValue = {});
}

namespace robot {

	template<typename T> Box getBoundingBox(const std::unordered_map<Position, T>& map)
	{
		auto minI = std::numeric_limits<std::int64_t>::max();
		auto maxI = std::numeric_limits<std::int64_t>::min();
		auto minJ = std::numeric_limits<std::int64_t>::max();
		auto maxJ = std::numeric_limits<std::int64_t>::min();

		for (auto&& entry : map) {
			minI = std::min(minI, entry.first.i);
			maxI = std::max(maxI, entry.first.i);
			minJ = std::min(minJ, entry.first.j);
			maxJ = std::max(maxJ, entry.first.j);
		}

		return { minI, maxI, minJ, maxJ };
	}

	template<typename T> image::Image<T> toImage(const std::unordered_map<Position, T>& map, const T& defaultValue)
	{
		const auto box = getBoundingBox(map);

		const auto width  = static_cast<size_t>(box.maxI - box.minI) + 1;
		const auto height = static_cast<size_t>(box.maxJ - box.minJ) + 1;

		auto image = image::Image<T>{ width, height, defaultValue };
		for (auto&& entry : map) {
			const auto i = static_cast<size_t>(entry.first.i - box.minI);
			const auto j = static_cast<size_t>(entry.first.j - box.minJ);
			image(i, j)  = entry.second;
		}
		return image;
	}
}