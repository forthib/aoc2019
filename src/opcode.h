#pragma once

#include <functional>
#include <vector>

namespace opcode
{
	std::vector<int> decode(std::vector<int> code);
	
	std::vector<int> decode(std::vector<int> code, std::vector<int>& inputs, std::vector<int>& outputs);
	
	std::vector<int> decode(std::vector<int> code, const std::function<int()>& inputFunction, const std::function<void(int)>& outputFunction);
}