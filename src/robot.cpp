#include "robot.h"

namespace robot {

	namespace {

		struct Box
		{
			std::int64_t minI, maxI, minJ, maxJ;
		};

		Box getBoundingBox(const std::unordered_map<robot::Position, std::int64_t>& surface)
		{
			auto minI = std::numeric_limits<std::int64_t>::max();
			auto maxI = std::numeric_limits<std::int64_t>::min();
			auto minJ = std::numeric_limits<std::int64_t>::max();
			auto maxJ = std::numeric_limits<std::int64_t>::min();

			for (auto&& entry : surface) {
				minI = std::min(minI, entry.first.i);
				maxI = std::max(maxI, entry.first.i);
				minJ = std::min(minJ, entry.first.j);
				maxJ = std::max(maxJ, entry.first.j);
			}

			return { minI, maxI, minJ, maxJ };
		}
	}

	void Robot::decode(std::int64_t value)
	{
		switch (value) {
		case 0:
			turnLeft();
			forward();
			break;
		case 1:
			turnRight();
			forward();
			break;
		default: break;
		}
	}

	void Robot::turnLeft()
	{
		switch (orientation_) {
		case Orientation::UP: orientation_ = Orientation::LEFT; break;
		case Orientation::LEFT: orientation_ = Orientation::DOWN; break;
		case Orientation::DOWN: orientation_ = Orientation::RIGHT; break;
		case Orientation::RIGHT: orientation_ = Orientation::UP; break;
		default: throw std::exception{ "unsupported orientation" };
		}
	}

	void Robot::turnRight()
	{
		switch (orientation_) {
		case Orientation::UP: orientation_ = Orientation::RIGHT; break;
		case Orientation::RIGHT: orientation_ = Orientation::DOWN; break;
		case Orientation::DOWN: orientation_ = Orientation::LEFT; break;
		case Orientation::LEFT: orientation_ = Orientation::UP; break;
		default: throw std::exception{ "unsupported orientation" };
		}
	}

	void Robot::forward()
	{
		switch (orientation_) {
		case Orientation::UP: --position_.j; break;
		case Orientation::DOWN: ++position_.j; break;
		case Orientation::LEFT: --position_.i; break;
		case Orientation::RIGHT: ++position_.i; break;
		default: throw std::exception{ "unsupported orientation" };
		}
	}

	PaintingRobot::PaintingRobot(std::int64_t firstValue) { surface_[robot_.getPosition()] = firstValue; }

	std::int64_t PaintingRobot::getCurrentValue() const
	{
		const auto it = surface_.find(robot_.getPosition());
		return it == surface_.end() ? 0 : it->second;
	}

	void PaintingRobot::decode(std::int64_t value)
	{
		if (painting_)
			surface_[robot_.getPosition()] = value;
		else
			robot_.decode(value);
		painting_ = !painting_;
	}

	image::Image<std::int64_t> PaintingRobot::getImage() const
	{
		const auto box = getBoundingBox(surface_);

		const auto width  = static_cast<size_t>(box.maxI - box.minI) + 1;
		const auto height = static_cast<size_t>(box.maxJ - box.minJ) + 1;

		auto image = image::Image<std::int64_t>{ width, height, 0 };
		for (auto&& entry : surface_) {
			const auto i = static_cast<size_t>(entry.first.i - box.minI);
			const auto j = static_cast<size_t>(entry.first.j - box.minJ);
			image(i, j)  = entry.second;
		}
		return image;
	}
}