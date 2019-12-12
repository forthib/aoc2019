#include "moon.h"

#include "io.h"
#include <sstream>

namespace moon {

	namespace {

		template<typename T> T gcd(T x, T y)
		{
			while (y != 0) {
				const auto r = x % y;

				x = y;
				y = r;
			}
			return x;
		}

		template<typename T> T lcm(T x, T y)
		{
			const auto div = gcd(x, y);
			return div ? (x / div * y) : 0;
		}

		size_t getNDigits(std::int64_t value)
		{
			std::ostringstream out;
			out << value;
			return out.str().size();
		}

		void writeBlanks(std::int64_t value, size_t maxDigits, std::ostream& out)
		{
			for (size_t i = getNDigits(value); i < maxDigits; ++i)
				out << " ";
		}

		void write(const Coord<3>& coord, std::ostream& out, const std::array<size_t, 3>& maxDigits,
		    const std::array<std::string, 4>& separators = { "<x=", ", y=", ", z=", ">" })
		{
			for (size_t axis = 0; axis < 3; ++axis) {
				out << separators[axis];
				writeBlanks(coord[axis], maxDigits[axis], out);
				out << coord[axis];
			}
			out << separators[3];
		}
	}

	System<3> read(const std::string& fileName)
	{
		const auto values = io::readLinesOfIntegers(fileName, "<>, xyz=");

		auto system = System<3>(values.size());
		for (size_t i = 0; i < values.size(); ++i)
			system.setPosition(i, { { values[i][0], values[i][1], values[i][2] } });
		return system;
	}

	System<1> select(const System<3>& system, size_t axis)
	{
		auto selected = System<1>(system.size());
		for (size_t i = 0; i < system.size(); ++i) {
			selected.setPosition(i, { { system.getPosition(i)[axis] } });
			selected.setVelocity(i, { { system.getVelocity(i)[axis] } });
		}
		return selected;
	}

	System<1> read(const std::string& fileName, size_t axis)
	{
		const auto values = io::readLinesOfIntegers(fileName, "<>, xyz=");

		auto system = System<1>(values.size());
		for (size_t i = 0; i < values.size(); ++i)
			system.setPosition(i, { { values[i][axis] } });
		return system;
	}

	size_t getTotalEnergy(const std::string& fileName, size_t maxStep)
	{ //
		return getTotalEnergy(read(fileName), maxStep);
	}

	size_t getTotalEnergy(System<3> system, size_t maxStep)
	{
		for (size_t i = 0; i < maxStep; ++i) {
			system.applyGravity();
			system.applyVelocity();
		}
		return system.getTotalEnergy();
	}

	size_t getPeriod(const std::string& fileName)
	{
		return getPeriod(read(fileName));
	}

	size_t getPeriod(System<1> system)
	{
		const auto originalSystem = system;

		size_t nSteps = 0;
		do {
			system.applyGravity();
			system.applyVelocity();
			++nSteps;
		} while (system != originalSystem);

		return nSteps;
	}

	size_t getPeriod(const System<3>& system)
	{
		const auto periodX = getPeriod(select(system, 0));
		const auto periodY = getPeriod(select(system, 1));
		const auto periodZ = getPeriod(select(system, 2));
		return lcm(periodX, lcm(periodY, periodZ));
	}

	void writePositionsAndVelocities(const System<3>& system, std::ostream& out)
	{
		const size_t n = system.size();

		auto maxDigitsPos = std::array<size_t, 3>{};
		for (size_t axis = 0; axis < 3; ++axis)
			for (size_t i = 0; i < n; ++i)
				maxDigitsPos[axis] = std::max(maxDigitsPos[axis], getNDigits(system.getPosition(i)[axis]));

		auto maxDigitsVel = std::array<size_t, 3>{};
		for (size_t axis = 0; axis < 3; ++axis)
			for (size_t i = 0; i < n; ++i)
				maxDigitsVel[axis] = std::max(maxDigitsVel[axis], getNDigits(system.getVelocity(i)[axis]));

		for (size_t i = 0; i < n; ++i) {
			out << "pos=";
			write(system.getPosition(i), out, maxDigitsPos);
			out << ", vel=";
			write(system.getVelocity(i), out, maxDigitsVel);
			out << "\n";
		}
	}

	void writeEnergies(const System<3>& system, std::ostream& out)
	{
		const size_t n = system.size();

		auto maxDigitsPos = std::array<size_t, 3>{};
		for (size_t axis = 0; axis < 3; ++axis)
			for (size_t i = 0; i < n; ++i)
				maxDigitsPos[axis] = std::max(maxDigitsPos[axis], getNDigits(std::abs(system.getPosition(i)[axis])));

		auto maxDigitsVel = std::array<size_t, 3>{};
		for (size_t axis = 0; axis < 3; ++axis)
			for (size_t i = 0; i < n; ++i)
				maxDigitsVel[axis] = std::max(maxDigitsVel[axis], getNDigits(std::abs(system.getVelocity(i)[axis])));

		size_t maxDigitsPot = 0;
		for (size_t i = 0; i < n; ++i)
			maxDigitsPot = std::max(maxDigitsPot, getNDigits(system.getPotentialEnergy(i)));

		size_t maxDigitsKin = 0;
		for (size_t i = 0; i < n; ++i)
			maxDigitsKin = std::max(maxDigitsKin, getNDigits(system.getKineticEnergy(i)));

		size_t maxDigitsTotal = 0;
		for (size_t i = 0; i < n; ++i)
			maxDigitsTotal = std::max(maxDigitsTotal, getNDigits(system.getTotalEnergy(i)));

		for (size_t i = 0; i < n; ++i) {
			out << "pot: ";
			write(abs(system.getPosition(i)), out, maxDigitsPos, { "", " + ", " + ", " = " });
			writeBlanks(system.getPotentialEnergy(i), maxDigitsPot, out);
			out << system.getPotentialEnergy(i);
			out << ";   kin: ";
			write(abs(system.getVelocity(i)), out, maxDigitsVel, { "", " + ", " + ", " = " });
			writeBlanks(system.getKineticEnergy(i), maxDigitsKin, out);
			out << system.getKineticEnergy(i);
			out << ";   total: ";
			writeBlanks(system.getPotentialEnergy(i), maxDigitsPot, out);
			out << system.getPotentialEnergy(i);
			out << " * ";
			writeBlanks(system.getKineticEnergy(i), maxDigitsKin, out);
			out << system.getKineticEnergy(i);
			out << " = ";
			writeBlanks(system.getTotalEnergy(i), maxDigitsTotal, out);
			out << system.getTotalEnergy(i);
			out << "\n";
		}

		out << "Sum of total energy: ";
		out << system.getTotalEnergy(0);
		for (size_t i = 1; i < n; ++i)
			out << " + " << system.getTotalEnergy(i);
		out << " = " << system.getTotalEnergy() << "\n";
	}

	void write(const std::string& fileName, const std::vector<size_t>& steps, std::ostream& out)
	{ //
		write(read(fileName), steps, out);
	}

	void write(System<3> system, const std::vector<size_t>& steps, std::ostream& out)
	{
		const size_t maxStep = *std::max_element(steps.begin(), steps.end());

		for (size_t i = 0; i < maxStep; ++i) {
			if (std::find(steps.begin(), steps.end(), i) != steps.end()) {
				out << "After " << i << " steps:\n";
				writePositionsAndVelocities(system, out);
				out << "\n";
			}
			system.applyGravity();
			system.applyVelocity();
		}
		out << "After " << maxStep << " steps:\n";
		writePositionsAndVelocities(system, out);
		out << "\n";
		out << "Energy after " << maxStep << " steps:\n";
		writeEnergies(system, out);
	}
}