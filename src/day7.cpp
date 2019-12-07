#include "opcode.h"
#include "io.h"

#include <algorithm>
#include <thread>
#include <tbb/concurrent_queue.h>
#include <tbb/task_group.h>

namespace
{
	class Amplifier
	{
	public:

		Amplifier(std::string name, int phase) : name_{ std::move(name) }, phase_{ phase }, bindedAmp_{ nullptr }
		{
		}

		~Amplifier() = default;
		Amplifier(const Amplifier&) = delete;
		Amplifier(Amplifier&&) = delete;
		Amplifier& operator=(const Amplifier&) = delete;
		Amplifier& operator=(Amplifier&&) = delete;

		void run(const std::vector<int>& code)
		{
			bool phaseInitialized = false;

			const auto inputFunction = [&]() {
				if (!phaseInitialized)
				{
					phaseInitialized = true;
					return phase_;
				}
				return getNextInput();
			};

			const auto outputFunction = [&](int value) {
				if (bindedAmp_)
					bindedAmp_->inputs_.push(value);
			};

			opcode::decode(code, inputFunction, outputFunction);
		}

		void addInput(int input)
		{
			inputs_.push(input);
		}
		
		void bindTo(Amplifier& bindedAmp)
		{
			bindedAmp_ = &bindedAmp;
		}

		int getNextInput()
		{
			int value;
			while (!inputs_.try_pop(value))
				std::this_thread::yield();
			return value;
		}

	private:

		friend void bind(Amplifier& amp1, Amplifier& amp2);

		std::string name_;
		int phase_;

		tbb::concurrent_queue<int> inputs_;

		Amplifier* bindedAmp_;
	};

	int getSignal(const std::vector<int>& code, const std::vector<int>& phaseSequence)
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

	void run(const std::vector<int>& code, std::vector<int> phaseSequence)
	{
		std::sort(phaseSequence.begin(), phaseSequence.end());

		int maxSignal = 0;
		auto maxSignalPhaseSequence = std::vector<int>{};
		do
		{
			const int signal = getSignal(code, phaseSequence);
			if (signal > maxSignal)
			{
				maxSignal = signal;
				maxSignalPhaseSequence = phaseSequence;
			}
		} while (std::next_permutation(phaseSequence.begin(), phaseSequence.end()));

		std::cout << "Max signal: " << maxSignal << " with phase sequence ";
		io::display(std::cout, maxSignalPhaseSequence);
	}

	void runPart1(const std::vector<int>& code)
	{
		run(code, { 0, 1, 2, 3, 4 });
	}

	void runPart2(const std::vector<int>& code)
	{
		run(code, { 5, 6, 7, 8, 9 });
	}
}

int main(int argc, char* argv[])
{
	std::cout << "Part 1\n";
	runPart1({ 3, 15, 3, 16, 1002, 16, 10, 16, 1, 16, 15, 15, 4, 15, 99, 0, 0 });
	runPart1({ 3, 23, 3, 24, 1002, 24, 10, 24, 1002, 23, -1, 23, 101, 5, 23, 23, 1, 24, 23, 23, 4, 23, 99, 0, 0 });
	runPart1({ 3, 31, 3, 32, 1002, 32, 10, 32, 1001, 31, -2, 31, 1007, 31, 0, 33, 1002, 33, 7, 33, 1, 33, 31, 31, 1, 32, 31, 31, 4, 31, 99, 0, 0, 0 });
	runPart1(io::readLineOfIntegers("day7_input.txt"));

	std::cout << "Part 2\n";
	runPart2({ 3, 26, 1001, 26, -4, 26, 3, 27, 1002, 27, 2, 27, 1, 27, 26, 27, 4, 27, 1001, 28, -1, 28, 1005, 28, 6, 99, 0, 0, 5 });
	runPart2({ 3, 52, 1001, 52, -5, 52, 3, 53, 1, 52, 56, 54, 1007, 54, 5, 55, 1005, 55, 26, 1001, 54, -5, 54, 1105, 1, 12, 1, 53, 54, 53, 1008, 54, 0, 55, 1001, 55, 1, 55, 2, 53, 55, 53, 4, 53, 1001, 56, -1, 56, 1005, 56, 6, 99, 0, 0, 0, 0, 10 });
	runPart2(io::readLineOfIntegers("day7_input.txt"));

	std::cin.get();
}
