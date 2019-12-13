#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace opcode {
	std::vector<std::int64_t> run(const std::vector<std::int64_t>& code);
	std::vector<std::int64_t> run(const std::vector<std::int64_t>& code, const std::vector<std::int64_t>& inputs);
	std::vector<std::int64_t> run(std::vector<std::int64_t>& code);
	std::vector<std::int64_t> run(std::vector<std::int64_t>& code, const std::vector<std::int64_t>& inputs);

	void run(const std::vector<std::int64_t>& code, std::vector<std::int64_t>& inputs, std::vector<std::int64_t>& outputs);
	void run(std::vector<std::int64_t>& code, std::vector<std::int64_t>& inputs, std::vector<std::int64_t>& outputs);
	void run(const std::vector<std::int64_t>& code, std::function<std::int64_t()> inputFunction, std::function<void(std::int64_t)> outputFunction);
	void run(std::vector<std::int64_t>& code, std::function<std::int64_t()> inputFunction, std::function<void(std::int64_t)> outputFunction);
}