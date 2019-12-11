#include "image.h"

#include <iostream>

namespace image {

	std::vector<Image<std::int64_t>> splitLayers(size_t width, size_t height, const std::vector<std::int64_t>& data)
	{
		const size_t nLayers = data.size() / (width * height);

		auto layers = std::vector<Image<std::int64_t>>(nLayers, { width, height });
		for (size_t iLayer = 0; iLayer < nLayers; ++iLayer) {
			for (size_t j = 0; j < height; ++j) {
				for (size_t i = 0; i < width; ++i) {
					const size_t dataIndex = iLayer * (width * height) + (j * width + i);
					layers[iLayer](i, j)   = data[dataIndex];
				}
			}
		}
		return layers;
	}

	size_t getCheckValue(const std::vector<Image<std::int64_t>>& layers)
	{
		size_t minNZeros  = std::numeric_limits<size_t>::max();
		size_t checkValue = 0;

		const size_t nLayers = layers.size();
		for (size_t iLayer = 0; iLayer < nLayers; ++iLayer) {
			const auto&  layer  = layers[iLayer];
			const size_t nZeros = std::count(layer.begin(), layer.end(), 0);
			const size_t nOnes  = std::count(layer.begin(), layer.end(), 1);
			const size_t nTwos  = std::count(layer.begin(), layer.end(), 2);

			if (nZeros < minNZeros) {
				minNZeros  = nZeros;
				checkValue = nOnes * nTwos;
			}
		}

		return checkValue;
	}

	Image<std::int64_t> decode(const std::vector<Image<std::int64_t>>& layers)
	{
		const size_t width  = layers.front().getWidth();
		const size_t height = layers.front().getHeight();

		auto decoded = Image<std::int64_t>{ width, height, 0 };
		for (size_t j = 0; j < height; ++j) {
			for (size_t i = 0; i < width; ++i) {
				for (const auto& layer : layers) {
					if (layer(i, j) != 2) {
						decoded(i, j) = layer(i, j);
						break;
					}
				}
			}
		}
		return decoded;
	}
}
