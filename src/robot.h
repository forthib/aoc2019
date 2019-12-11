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
}