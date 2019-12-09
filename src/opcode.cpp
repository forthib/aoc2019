#include "opcode.h"

#include <iostream>
#include <unordered_map>

namespace opcode {
	namespace {

		class Program
		{
		public:
			Program(std::vector<std::int64_t> code, std::function<std::int64_t()> inputFunction, std::function<void(std::int64_t)> outputFunction)
			    : code_{ std::move(code) }, inputFunction_{ std::move(inputFunction) }, outputFunction_{ std::move(outputFunction) }
			{
			}

			Program& run();

			std::vector<std::int64_t> releaseCode() { return std::move(code_); }

		private:
			struct PModes
			{
				std::int64_t pMode1, pMode2, pMode3;
			};

			PModes        decodePModes();
			std::int64_t  getValuePosition(std::int64_t position, std::int64_t pMode);
			std::int64_t& getValue(std::int64_t position, std::int64_t pMode);

			template<typename Operator> void  binaryOp(Operator op);
			template<typename Predicate> void jumpIf(Predicate pred);

			void add();
			void multiply();
			void readInput();
			void writeOutput();
			void jumpIfTrue();
			void jumpIfFalse();
			void lessThan();
			void equals();
			void adjustRelativeBase();

			std::vector<std::int64_t>         code_;
			std::int64_t                      position_     = 0;
			std::int64_t                      relativeBase_ = 0;
			std::function<std::int64_t()>     inputFunction_;
			std::function<void(std::int64_t)> outputFunction_;
		};

		Program& Program::run()
		{
			for (;;) {
				const auto instruction = code_[position_] % 100;
				if (instruction == 99)
					break;

				switch (instruction) {
				case 1: add(); break;
				case 2: multiply(); break;
				case 3: readInput(); break;
				case 4: writeOutput(); break;
				case 5: jumpIfTrue(); break;
				case 6: jumpIfFalse(); break;
				case 7: lessThan(); break;
				case 8: equals(); break;
				case 9: adjustRelativeBase(); break;
				default: throw std::exception{ "unsupported opcode" };
				}
			}
			return *this;
		}

		Program::PModes Program::decodePModes()
		{
			size_t opCode = code_[position_];
			opCode /= 100;
			const std::int64_t pMode1 = opCode % 10;
			opCode /= 10;
			const std::int64_t pMode2 = opCode % 10;
			opCode /= 10;
			const std::int64_t pMode3 = opCode % 10;

			return { pMode1, pMode2, pMode3 };
		}

		std::int64_t Program::getValuePosition(std::int64_t position, std::int64_t pMode)
		{
			switch (pMode) {
			case 0: return code_[position];
			case 1: return position;
			case 2: return code_[position] + relativeBase_;
			default: throw std::exception{ "unsupported parameter mode" };
			}
		}

		std::int64_t& Program::getValue(std::int64_t position, std::int64_t pMode)
		{
			position = getValuePosition(position, pMode);
			if (static_cast<size_t>(position) >= code_.size())
				code_.resize(static_cast<size_t>(position + 1), 0);
			return code_[position];
		}

		template<typename Operator> void Program::binaryOp(Operator op)
		{
			const auto pModes = decodePModes();

			if (pModes.pMode3 == 1)
				throw std::exception{ "unsupported parameter mode for third argument" };

			const auto value1 = getValue(position_ + 1, pModes.pMode1);
			const auto value2 = getValue(position_ + 2, pModes.pMode2);
			auto&      value3 = getValue(position_ + 3, pModes.pMode3);

			value3 = op(value1, value2);
			position_ += 4;
		}

		template<typename Predicate> void Program::jumpIf(Predicate pred)
		{
			const auto pModes = decodePModes();

			const auto value1 = getValue(position_ + 1, pModes.pMode1);
			const auto value2 = getValue(position_ + 2, pModes.pMode2);

			if (pred(value1))
				position_ = value2;
			else
				position_ += 3;
		}

		void Program::add()
		{
			binaryOp([](auto a, auto b) { return a + b; });
		}

		void Program::multiply()
		{
			binaryOp([](auto a, auto b) { return a * b; });
		}

		void Program::readInput()
		{
			const auto pModes = decodePModes();

			if (pModes.pMode1 == 1)
				throw std::exception{ "unsupported parameter mode for first argument" };

			auto& value = getValue(position_ + 1, pModes.pMode1);
			value       = inputFunction_();

			position_ += 2;
		}

		void Program::writeOutput()
		{
			const auto pModes = decodePModes();

			const auto value = getValue(position_ + 1, pModes.pMode1);
			outputFunction_(value);

			position_ += 2;
		}

		void Program::jumpIfTrue()
		{
			return jumpIf([](auto value) { return value != 0; });
		}

		void Program::jumpIfFalse()
		{
			return jumpIf([](auto value) { return value == 0; });
		}

		void Program::lessThan()
		{
			binaryOp([](auto a, auto b) { return a < b ? 1 : 0; });
		}

		void Program::equals()
		{
			binaryOp([](auto a, auto b) { return a == b ? 1 : 0; });
		}

		void Program::adjustRelativeBase()
		{
			const auto pModes = decodePModes();

			const auto value = getValue(position_ + 1, pModes.pMode1);
			relativeBase_ += value;

			position_ += 2;
		}
	}

	std::vector<std::int64_t> run(const std::vector<std::int64_t>& code)
	{
		auto codeCopy = code;
		return run(codeCopy, {});
	}

	std::vector<std::int64_t> run(const std::vector<std::int64_t>& code, const std::vector<std::int64_t>& inputs)
	{
		auto codeCopy = code;
		return run(codeCopy, inputs);
	}

	std::vector<std::int64_t> run(std::vector<std::int64_t>& code)
	{ //
		return run(code, {});
	}

	std::vector<std::int64_t> run(std::vector<std::int64_t>& code, const std::vector<std::int64_t>& inputs)
	{
		auto inputsCopy = inputs;
		auto outputs    = std::vector<std::int64_t>{};
		run(code, inputsCopy, outputs);
		return outputs;
	}

	void run(std::vector<std::int64_t>& code, std::vector<std::int64_t>& inputs, std::vector<std::int64_t>& outputs)
	{
		const auto inputFunction = [&inputs]() {
			const auto value = inputs.front();
			inputs.erase(inputs.begin());
			return value;
		};
		const auto outputFunction = [&outputs](std::int64_t value) { outputs.push_back(value); };
		run(code, inputFunction, outputFunction);
	}

	void run(std::vector<std::int64_t>& code, std::function<std::int64_t()> inputFunction, std::function<void(std::int64_t)> outputFunction)
	{ //
		code = Program{ std::move(code), std::move(inputFunction), std::move(outputFunction) }.run().releaseCode();
	}
}
