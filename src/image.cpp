#include "image.h"

#include <iostream>

namespace image
{
	namespace
	{
		std::vector<std::vector<int>> splitLayers(size_t width, size_t height, const std::vector<int>& data)
		{
			const size_t nLayers = data.size() / (width * height);

			auto layers = std::vector<std::vector<int>>(nLayers);
			for (size_t iLayer = 0; iLayer < nLayers; ++iLayer)
			{
				layers[iLayer].resize(width * height);
				for (size_t j = 0; j < height; ++j)
				{
					for (size_t i = 0; i < width; ++i)
					{
						const size_t dataIndex = iLayer * (width * height) + (j * width + i);
						const size_t dataLayerIndex = (j * width + i);
						layers[iLayer][dataLayerIndex] = data[dataIndex];
					}
				}
			}
			return layers;
		}

		std::vector<int> stack(size_t width, size_t height, const std::vector<std::vector<int>>& layers)
		{
			const size_t nLayers = layers.size();

			auto stacked = std::vector<int>(width * height, 0);
			for (size_t j = 0; j < height; ++j)
			{
				for (size_t i = 0; i < width; ++i)
				{
					for (const auto& layer : layers)
					{
						const size_t index = j * width + i;
						if (layer[index] != 2)
						{
							stacked[index] = layer[index];
							break;
						}
					}
				}
			}
			return stacked;
		}
	}

	void check(size_t width, size_t height, const std::vector<int>& data)
	{
		const auto layers = splitLayers(width, height, data);

		size_t minNZeros = std::numeric_limits<size_t>::max();
		size_t checkValue = 0;

		const size_t nLayers = layers.size();
		for (size_t iLayer = 0; iLayer < nLayers; ++iLayer)
		{
			const auto& layer = layers[iLayer];
			const size_t nZeros = std::count(layer.begin(), layer.end(), 0);
			const size_t nOnes = std::count(layer.begin(), layer.end(), 1);
			const size_t nTwos = std::count(layer.begin(), layer.end(), 2);

			if (nZeros < minNZeros)
			{
				minNZeros = nZeros;
				checkValue = nOnes * nTwos;
			}
		}

		std::cout << "Check value: " << checkValue << "\n\n";
	}

	void displayRaw(size_t width, size_t height, const std::vector<int>& data)
	{
		const auto layers = splitLayers(width, height, data);

		const size_t nLayers = layers.size();
		for (size_t iLayer = 0; iLayer < nLayers; ++iLayer)
		{
			std::cout << "Layer " << iLayer << "\n\n";

			for (size_t j = 0; j < height; ++j)
			{
				std::cout << "\t";
				for (size_t i = 0; i < width; ++i)
					std::cout << layers[iLayer][j * width + i];
				std::cout << "\n";
			}
			std::cout << "\n";
		}
	}

	void displayDecoded(size_t width, size_t height, const std::vector<int>& data, bool pretty)
	{
		const auto layers = splitLayers(width, height, data);
		const auto stackedData = stack(width, height, layers);

		std::cout << "Decoded image\n\n";

		for (size_t j = 0; j < height; ++j)
		{
			std::cout << "\t";
			for (size_t i = 0; i < width; ++i)
				if (pretty)
					std::cout << (stackedData[j * width + i] ? '#' : ' ');
				else
					std::cout << stackedData[j * width + i];
			std::cout << "\n";
		}
		std::cout << "\n";
	}
}
