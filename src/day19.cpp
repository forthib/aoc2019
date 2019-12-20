#include "image.h"
#include "io.h"
#include "opcode.h"
#include "test.h"
#include <boost/optional.hpp>

namespace {
	int64_t getFieldAt(const std::vector<int64_t>& code, size_t i, size_t j)
	{
		return opcode::run(code, { static_cast<int64_t>(i), static_cast<int64_t>(j) }).back();
	}

	image::Image<int64_t> getField(const std::vector<int64_t>& code, size_t width, size_t height)
	{
		auto field = image::Image<int64_t>{ width, height };
		for (size_t j = 0; j < height; ++j)
			for (size_t i = 0; i < width; ++i)
				field(i, j) = getFieldAt(code, i, j);
		return field;
	}

	bool isSquare(const std::vector<int64_t>& code, int64_t value, size_t i, size_t j, size_t size)
	{
		if (getFieldAt(code, i, j) != value)
			return false;
		if (getFieldAt(code, i + size - 1, j) != value)
			return false;
		if (getFieldAt(code, i, j + size - 1) != value)
			return false;
		if (getFieldAt(code, i + size - 1, j + size - 1) != value)
			return false;

		for (size_t y = j; y < j + size; ++y)
			for (size_t x = i; x < i + size; ++x)
				if (getFieldAt(code, x, y) != value)
					return false;

		return true;
	}

	image::IJ findSquare(const std::vector<int64_t>& code, int64_t value, size_t size)
	{
		const size_t halfSize = size / 2;

		size_t minDistance = std::numeric_limits<size_t>::max();
		auto   result      = boost::optional<image::IJ>{};

		for (size_t ij = 1;; ++ij) {
			for (size_t j = 1; j <= ij; ++j) {
				const size_t i = ij - j;
				if (isSquare(code, value, i * halfSize, j * halfSize, halfSize)) {

					const size_t minX = (i - 1) * halfSize;
					const size_t minY = (j - 1) * halfSize;

					for (size_t y = minY; y < minY + size; ++y) {
						for (size_t x = minX; x < minX + size; ++x) {
							if (isSquare(code, value, x, y, size)) {
								if (x * x + y * y < minDistance) {
									minDistance = x * x + y * y;
									result      = image::IJ{ x, y };
								}
							}
						}
					}
				}
			}
			if (result)
				return *result;
		}
		throw std::exception{ "not found" };
	}
}

int main(int argc, char* argv[])
{
	const auto code    = io::readLineOfIntegers("day19_input.txt");
	const auto field50 = getField(code, 50, 50);
	const auto count50 = std::count(field50.begin(), field50.end(), 1);
	const auto pos100  = findSquare(code, 1, 100);

	std::cout << "Part 1: " << count50 << "\n";
	std::cout << "Part 2: " << 10000 * pos100.i + pos100.j << "\n";

	std::cin.get();
}
