#include "io.h"
#include "opcode.h"
#include "test.h"

#include <algorithm>
#include <tbb/concurrent_queue.h>
#include <tbb/task_group.h>
#include <thread>

namespace {
	class Amplifier
	{
	public:
		Amplifier(std::string name, std::int64_t phase) : name_{ std::move(name) }, phase_{ phase }, bindedAmp_{ nullptr } {}

		~Amplifier()                = default;
		Amplifier(const Amplifier&) = delete;
		Amplifier(Amplifier&&)      = delete;
		Amplifier& operator=(const Amplifier&) = delete;
		Amplifier& operator=(Amplifier&&) = delete;

		void run(std::vector<std::int64_t> code)
		{
			bool phaseInitialized = false;

			const auto inputFunction = [&]() {
				if (!phaseInitialized) {
					phaseInitialized = true;
					return phase_;
				}
				return getNextInput();
			};

			const auto outputFunction = [&](std::int64_t value) {
				if (bindedAmp_)
					bindedAmp_->inputs_.push(value);
			};

			opcode::run(code, inputFunction, outputFunction);
		}

		void addInput(std::int64_t input) { inputs_.push(input); }

		void bindTo(Amplifier& bindedAmp) { bindedAmp_ = &bindedAmp; }

		std::int64_t getNextInput()
		{
			std::int64_t value;
			while (!inputs_.try_pop(value))
				std::this_thread::yield();
			return value;
		}

	private:
		friend void bind(Amplifier& amp1, Amplifier& amp2);

		std::string  name_;
		std::int64_t phase_;

		tbb::concurrent_queue<std::int64_t> inputs_;

		Amplifier* bindedAmp_;
	};

	std::int64_t getSignal(const std::vector<std::int64_t>& code, const std::vector<std::int64_t>& phaseSequence)
	{
		Amplifier A{ "A", phaseSequence[0] };
		Amplifier B{ "B", phaseSequence[1] };
		Amplifier C{ "C", phaseSequence[2] };
		Amplifier D{ "D", phaseSequence[3] };
		Amplifier E{ "E", phaseSequence[4] };

		A.addInput(0);
		A.bindTo(B);
		B.bindTo(C);
		C.bindTo(D);
		D.bindTo(E);
		E.bindTo(A);

		tbb::task_group g;
		g.run([&] { A.run(code); });
		g.run([&] { B.run(code); });
		g.run([&] { C.run(code); });
		g.run([&] { D.run(code); });
		g.run([&] { E.run(code); });
		g.wait();

		return A.getNextInput();
	}

	std::int64_t getMaxSignal(const std::vector<std::int64_t>& code, std::vector<std::int64_t> phaseSequence)
	{
		std::sort(phaseSequence.begin(), phaseSequence.end());

		std::int64_t maxSignal = 0;
		do {
			const auto signal = getSignal(code, phaseSequence);
			if (signal > maxSignal)
				maxSignal = signal;
		} while (std::next_permutation(phaseSequence.begin(), phaseSequence.end()));

		return maxSignal;
	}

	std::int64_t getMaxSignal1(const std::vector<std::int64_t>& code) { return getMaxSignal(code, { 0, 1, 2, 3, 4 }); }
	std::int64_t getMaxSignal2(const std::vector<std::int64_t>& code) { return getMaxSignal(code, { 5, 6, 7, 8, 9 }); }
}

int main(int argc, char* argv[])
{
	test::equals(getMaxSignal1({ 3, 15, 3, 16, 1002, 16, 10, 16, 1, 16, 15, 15, 4, 15, 99, 0, 0 }), 43210);
	test::equals(getMaxSignal1({ 3, 23, 3, 24, 1002, 24, 10, 24, 1002, 23, -1, 23, 101, 5, 23, 23, 1, 24, 23, 23, 4, 23, 99, 0, 0 }), 54321);
	test::equals(
	    getMaxSignal1({ 3, 31, 3, 32, 1002, 32, 10, 32, 1001, 31, -2, 31, 1007, 31, 0, 33, 1002, 33, 7, 33, 1, 33, 31, 31, 1, 32, 31, 31, 4, 31, 99, 0, 0, 0 }),
	    65210);

	test::equals(
	    getMaxSignal2({ 3, 26, 1001, 26, -4, 26, 3, 27, 1002, 27, 2, 27, 1, 27, 26, 27, 4, 27, 1001, 28, -1, 28, 1005, 28, 6, 99, 0, 0, 5 }), 139629729);
	test::equals(getMaxSignal2({ 3, 52, 1001, 52, -5, 52, 3, 53, 1, 52, 56, 54, 1007, 54, 5, 55, 1005, 55, 26, 1001, 54, -5, 54, 1105, 1, 12, 1, 53, 54, 53,
	                 1008, 54, 0, 55, 1001, 55, 1, 55, 2, 53, 55, 53, 4, 53, 1001, 56, -1, 56, 1005, 56, 6, 99, 0, 0, 0, 0, 10 }),
	    18216);

	const auto code = io::readLineOfIntegers("day7_input.txt");
	std::cout << "Part 1: " << getMaxSignal1(code) << "\n";
	std::cout << "Part 2: " << getMaxSignal2(code) << "\n";

	std::cin.get();
}
