#include "robot.h"

namespace robot {

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

	image::Image<std::int64_t> PaintingRobot::getImage() const { return toImage(surface_); }
}