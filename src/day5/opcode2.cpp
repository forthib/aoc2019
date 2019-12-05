#include "opcode2.h"

#include <iostream>

namespace opcode2
{
	namespace
	{
		struct PModes
		{
			int pMode1, pMode2, pMode3;
		};

		PModes decodePModes(int opCode)
		{
			opCode /= 100;
			const int pMode1 = opCode % 10;
			opCode /= 10;
			const int pMode2 = opCode % 10;
			opCode /= 10;
			const int pMode3 = opCode % 10;

			return { pMode1, pMode2, pMode3 };
		}

		int& getValue(std::vector<int>& code, int position, int pMode)
		{
			return pMode == 0 ? code[code[position]] : code[position];
		}

		template<typename Operator>
		void binaryOp(std::vector<int>& code, int& position, Operator op)
		{
			const auto pModes = decodePModes(code[position]);

			if (pModes.pMode3 != 0)
				throw std::exception{ "unsupported parameter mode for third argument" };

			const int value1 = getValue(code, position + 1, pModes.pMode1);
			const int value2 = getValue(code, position + 2, pModes.pMode2);
			int& value3 = getValue(code, position + 3, pModes.pMode3);

			value3 = op(value1, value2);
			position += 4;
		}

		void add(std::vector<int>& code, int& position)
		{
			binaryOp(code, position, [](auto a, auto b) { return a + b; });
		}

		void multiply(std::vector<int>& code, int& position)
		{
			binaryOp(code, position, [](auto a, auto b) { return a * b; });
		}

		void readInput(std::vector<int>& code, int& position)
		{
			const auto pModes = decodePModes(code[position]);

			if (pModes.pMode1 != 0)
				throw std::exception{ "unsupported parameter mode for first argument" };
			
			std::cout << "Input:";
			int input;
			std::cin >> input;
			
			int& value = getValue(code, position + 1, pModes.pMode1);
			value = input;

			position += 2;
		}

		void writeOutput(std::vector<int>& code, int& position)
		{
			const auto pModes = decodePModes(code[position]);

			const int value = getValue(code, position + 1, pModes.pMode1);

			std::cout << value << "\n";

			position += 2;
		}

		template<typename Predicate>
		void jumpIf(std::vector<int>& code, int& position, Predicate pred)
		{
			const auto pModes = decodePModes(code[position]);
			
			const int value1 = getValue(code, position + 1, pModes.pMode1);
			const int value2 = getValue(code, position + 2, pModes.pMode2);

			if (pred(value1))
				position = value2;
			else
				position += 3;
		}

		void jumpIfTrue(std::vector<int>& code, int& position)
		{
			return jumpIf(code, position, [](auto value) { return value != 0; });
		}

		void jumpIfFalse(std::vector<int>& code, int& position)
		{
			return jumpIf(code, position, [](auto value) { return value == 0; });
		}

		void lessThan(std::vector<int>& code, int& position)
		{
			binaryOp(code, position, [](auto a, auto b) { return a < b ? 1 : 0; });
		}

		void equals(std::vector<int>& code, int& position)
		{
			binaryOp(code, position, [](auto a, auto b) { return a == b ? 1 : 0; });
		}
	}


	std::vector<int> decode(std::vector<int> code)
	{
		int position = 0;
		for(;;)
		{
			const int instruction = code[position] % 100;
			if (instruction == 99)
				break;

			switch (instruction)
			{
			case 1:
				add(code, position);
				break;
			case 2:
				multiply(code, position);
				break;
			case 3:
				readInput(code, position);
				break;
			case 4:
				writeOutput(code, position);
				break;
			case 5:
				jumpIfTrue(code, position);
				break;
			case 6:
				jumpIfFalse(code, position);
				break;
			case 7:
				lessThan(code, position);
				break;
			case 8:
				equals(code, position);
				break;
			default:
				throw std::exception{ "unsupported opcode" };
			}
		}
		return code;
	}
}
