#include "opcode.h"

namespace opcode
{
	std::vector<size_t> decode(std::vector<size_t> code)
	{
		size_t position = 0;
		while (code[position] != 99)
		{
			const size_t opCode = code[position];
			const size_t position1 = code[position + 1];
			const size_t position2 = code[position + 2];
			const size_t position3 = code[position + 3];
			switch (opCode)
			{
			case 1:
				code[position3] = code[position1] + code[position2];
				break;
			case 2:
				code[position3] = code[position1] * code[position2];
				break;
			default:
				throw std::exception{ "unsupported opcode" };
			}
			position += 4;
		}
		return code;
	}
}