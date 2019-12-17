#include "io.h"
#include "test.h"

#include <array>
#include <numeric>

namespace {

	std::vector<std::int64_t> toArray(const std::string& signal)
	{
		auto result = std::vector<std::int64_t>{};
		result.reserve(signal.size());
		for (const auto c : signal)
			result.push_back(c - '0');
		return result;
	}

	std::string toString(const std::vector<std::int64_t>& signal)
	{
		auto result = std::string{};
		result.reserve(signal.size());
		for (const auto value : signal)
			result.push_back(static_cast<char>('0' + value));
		return result;
	}

	std::vector<std::int64_t> getSubArray(const std::vector<std::int64_t>& signal, size_t from, size_t to)
	{
		auto output = std::vector<std::int64_t>{};
		output.reserve(to - from);
		for (size_t i = from; i < to; ++i)
			output.push_back(signal[i % signal.size()]);
		return output;
	}

	size_t getOffset(const std::vector<std::int64_t>& signal)
	{
		size_t offset = 0;
		for (size_t i = 0; i < 7; ++i)
			offset = offset * 10 + signal[i];
		return offset;
	}

	template<typename Iterator> std::int64_t process(Iterator first, Iterator last, size_t nPatterns)
	{
		const size_t signalSize = std::distance(first, last);

		size_t nPeriods = signalSize / (4 * nPatterns);
		if (signalSize % (4 * nPatterns))
			++nPeriods;

		std::int64_t value = 0;
		for (size_t iPeriod = 0; iPeriod < nPeriods; ++iPeriod) {
			auto posFirstI = iPeriod * (4 * nPatterns);
			auto posLastI  = posFirstI + nPatterns;
			auto negFirstI = posLastI + nPatterns;
			auto negLastI  = negFirstI + nPatterns;

			posFirstI = std::min(posFirstI, signalSize);
			posLastI  = std::min(posLastI, signalSize);
			negFirstI = std::min(negFirstI, signalSize);
			negLastI  = std::min(negLastI, signalSize);

			for (size_t i = posFirstI; i < posLastI; ++i)
				value += first[i];

			for (size_t i = negFirstI; i < negLastI; ++i)
				value -= first[i];
		}

		value = std::abs(value);
		value = value % 10;

		return value;
	}

	std::vector<std::int64_t> fft(const std::vector<std::int64_t>& signal)
	{
		auto output = std::vector<std::int64_t>(signal.size());
		for (size_t i = 0, n = signal.size(); i < n; ++i)
			output[i] = process(signal.begin() + i, signal.end(), i + 1);
		return output;
	}

	std::vector<std::int64_t> fft(std::vector<std::int64_t> signal, size_t nPhases)
	{
		for (size_t i = 0; i < nPhases; ++i)
			signal = fft(signal);

		return getSubArray(signal, 0, 8);
	}

	std::vector<std::int64_t> fft2(std::vector<std::int64_t> signal, size_t nSignals, size_t nPhases)
	{
		const size_t offset = getOffset(signal);
		if (offset < signal.size() * nSignals / 2)
			throw std::exception{ "unsupported offset number :(" };

		const size_t n = signal.size() * nSignals - offset;

		auto values = std::vector<std::int64_t>(n, 1);
		for (size_t i = 1; i < nPhases; ++i) {
			std::partial_sum(values.begin(), values.end(), values.begin());
			for (auto& value : values)
				value = value % 10;
		}

		auto output = std::vector<std::int64_t>(8, 0);
		for (size_t iOutput = 0; iOutput < 8; ++iOutput) {
			for (size_t i = 0; i < values.size() - iOutput; ++i) {
				output[iOutput] += signal[(offset + iOutput + i) % signal.size()] * values[i];
				output[iOutput] = output[iOutput] % 10;
			}
		}

		return output;
	}

	std::string fft(const std::string& signal, size_t nPhases)
	{ //
		return toString(fft(toArray(signal), nPhases));
	}

	std::string fft2(const std::string& signal, size_t nSignals, size_t nPhases)
	{ //
		return toString(fft2(toArray(signal), nSignals, nPhases));
	}
}

int main(int argc, char* argv[])
{
	const auto input = io::readLineOfDigits("day16_input.txt");

	test::equals(fft("12345678", 1), "48226158");
	test::equals(fft("12345678", 2), "34040438");
	test::equals(fft("12345678", 3), "03415518");
	test::equals(fft("12345678", 4), "01029498");

	test::equals(fft("80871224585914546619083218645595", 100), "24176176");
	test::equals(fft("19617804207202209144916044189917", 100), "73745418");
	test::equals(fft("69317163492948606335995924319873", 100), "52432133");
	
	test::equals(fft2("03036732577212944063491565474664", 10000, 100), "84462026");
	test::equals(fft2("02935109699940807407585447034323", 10000, 100), "78725270");
	test::equals(fft2("03081770884921959731165446850517", 10000, 100), "53553731");

	std::cout << "Part 1: " << toString(fft(input, 100)) << "\n";
	std::cout << "Part 2: " << toString(fft2(input, 10000, 100)) << "\n";

	std::cin.get();
}