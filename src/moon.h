#pragma once

#include "coord.h"
#include <iostream>
#include <string>
#include <vector>

namespace moon {

	template<size_t Dim> using Coord = coord::Coord<Dim>;

	template<size_t Dim> class System
	{
	public:
		using Coord = Coord<Dim>;

		System(size_t n) : pos_(n), vel_(n) {}

		size_t size() const { return pos_.size(); }

		void setPosition(size_t i, const Coord& position) { pos_[i] = position; }
		void setVelocity(size_t i, const Coord& velocity) { vel_[i] = velocity; }

		const Coord& getPosition(size_t i) const { return pos_[i]; }
		const Coord& getVelocity(size_t i) const { return vel_[i]; }

		void applyGravity();
		void applyVelocity();

		std::int64_t getPotentialEnergy(size_t i) const;
		std::int64_t getKineticEnergy(size_t i) const;
		std::int64_t getTotalEnergy(size_t i) const;
		std::int64_t getTotalEnergy() const;

		auto asTuple() const { return std::tie(pos_, vel_); }

		friend bool equals(const System& lhs, const System& rhs) { return lhs.asTuple() == rhs.asTuple(); }
		friend bool operator!=(const System& lhs, const System& rhs) { return lhs.asTuple() != rhs.asTuple(); }

	private:
		std::vector<Coord> pos_;
		std::vector<Coord> vel_;
	};

	System<3> read(const std::string& fileName);
	System<1> select(const System<3>& system, size_t axis);

	size_t getTotalEnergy(const std::string& fileName, size_t maxStep);
	size_t getTotalEnergy(System<3> system, size_t maxStep);

	size_t getPeriod(const std::string& fileName);
	size_t getPeriod(const System<3>& system);
	size_t getPeriod(System<1> system);

	void writePositionsAndVelocities(const System<3>& system, std::ostream& out = std::cout);
	void writeEnergies(const System<3>& system, std::ostream& out = std::cout);

	void write(const std::string& fileName, const std::vector<size_t>& steps, std::ostream& out = std::cout);
	void write(System<3> system, const std::vector<size_t>& steps, std::ostream& out = std::cout);
}

namespace moon {

	template<size_t Dim> void System<Dim>::applyGravity()
	{
		const size_t n = size();
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = i + 1; j < n; ++j) {

				const auto& pos1 = pos_[i];
				const auto& pos2 = pos_[j];
				auto&       vel1 = vel_[i];
				auto&       vel2 = vel_[j];

				for (size_t axis = 0; axis < Dim; ++axis) {
					if (pos1[axis] < pos2[axis]) {
						++vel1[axis];
						--vel2[axis];
					}
					else if (pos1[axis] > pos2[axis]) {
						--vel1[axis];
						++vel2[axis];
					}
				}
			}
		}
	}

	template<size_t Dim> void System<Dim>::applyVelocity()
	{
		const size_t n = size();
		for (size_t i = 0; i < n; ++i)
			pos_[i] += vel_[i];
	}

	template<size_t Dim> std::int64_t System<Dim>::getPotentialEnergy(size_t i) const
	{
		std::int64_t energy = 0;
		for (size_t axis = 0; axis < Dim; ++axis)
			energy += std::abs(pos_[i][axis]);
		return energy;
	}

	template<size_t Dim> std::int64_t System<Dim>::getKineticEnergy(size_t i) const
	{
		std::int64_t energy = 0;
		for (size_t axis = 0; axis < Dim; ++axis)
			energy += std::abs(vel_[i][axis]);
		return energy;
	}

	template<size_t Dim> std::int64_t System<Dim>::getTotalEnergy(size_t i) const
	{ //
		return getPotentialEnergy(i) * getKineticEnergy(i);
	}

	template<size_t Dim> std::int64_t System<Dim>::getTotalEnergy() const
	{
		const size_t n      = size();
		std::int64_t energy = 0;
		for (size_t i = 0; i < n; ++i)
			energy += getTotalEnergy(i);
		return energy;
	}
}
