#pragma once

#include <boost/functional/hash.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

namespace image {

	struct IJ
	{
		size_t i = 0, j = 0;
	};

	inline decltype(auto) toTuple(IJ& ij) { return std::tie(ij.i, ij.j); }
	inline decltype(auto) toTuple(const IJ& ij) { return std::tie(ij.i, ij.j); }

	inline bool operator==(const IJ& lhs, const IJ& rhs) { return toTuple(lhs) == toTuple(rhs); }
	inline bool operator!=(const IJ& lhs, const IJ& rhs) { return toTuple(lhs) != toTuple(rhs); }
	inline bool operator<(const IJ& lhs, const IJ& rhs) { return toTuple(lhs) < toTuple(rhs); }
	inline bool operator>(const IJ& lhs, const IJ& rhs) { return toTuple(lhs) > toTuple(rhs); }
	inline bool operator<=(const IJ& lhs, const IJ& rhs) { return toTuple(lhs) <= toTuple(rhs); }
	inline bool operator>=(const IJ& lhs, const IJ& rhs) { return toTuple(lhs) >= toTuple(rhs); }

	template<typename T> class Image
	{
	public:
		Image(size_t width, size_t height, T defaultValue = {}) : width_{ width }, height_{ height }, data_(width * height, defaultValue) {}
		Image(size_t width, size_t height, std::vector<T> data) : width_{ width }, height_{ height }, data_{ std::move(data) } {}

		size_t getWidth() const { return width_; }
		size_t getHeight() const { return height_; }
		size_t size() const { return data_.size(); }

		size_t getIndex(size_t i, size_t j) const { return j * width_ + i; }
		size_t getIndex(const IJ& ij) const { return getIndex(ij.i, ij.j); }
		IJ     getIJ(size_t index) const { return { index % width_, index / width_ }; }

		decltype(auto) operator()(size_t index) const { return data_[index]; }
		decltype(auto) operator()(size_t index) { return data_[index]; }
		decltype(auto) operator()(size_t i, size_t j) const { return data_[getIndex(i, j)]; }
		decltype(auto) operator()(size_t i, size_t j) { return data_[getIndex(i, j)]; }
		decltype(auto) operator()(const IJ& ij) const { return data_[getIndex(ij)]; }
		decltype(auto) operator()(const IJ& ij) { return data_[getIndex(ij)]; }

		decltype(auto) begin() const { return data_.begin(); }
		decltype(auto) begin() { return data_.begin(); }
		decltype(auto) end() const { return data_.end(); }
		decltype(auto) end() { return data_.end(); }

	private:
		size_t         width_, height_;
		std::vector<T> data_;
	};

	template<typename T>
	void display(const Image<T>& image, std::ostream& out = std::cout, const std::string& prefix = "", const std::string& suffix = "\n",
	    const std::string& separator = "");

	template<typename T, typename U, typename Function> Image<T> transformIJ(const Image<U>& inputImage, Function function);
	template<typename T, typename U, typename Function> Image<T> transform(const Image<U>& inputImage, Function function);

	template<typename T, typename U, typename V, typename Function>
	Image<T> transformIJ(const Image<U>& inputImage1, const Image<V>& inputImage2, Function function);
	template<typename T, typename U, typename V, typename Function>
	Image<T> transform(const Image<U>& inputImage1, const Image<V>& inputImage2, Function function);

	template<typename T> Image<T> resized(const Image<T>& image, size_t width, size_t height, T defaultValue = {});
	template<typename T> void     resize(Image<T>& image, size_t width, size_t height, T defaultValue = {});

	std::vector<Image<std::int64_t>> splitLayers(size_t width, size_t height, const std::vector<std::int64_t>& data);

	size_t getCheckValue(const std::vector<Image<std::int64_t>>& layers);

	Image<std::int64_t> decode(const std::vector<Image<std::int64_t>>& layers);
}

namespace std {
	template<> struct hash<image::IJ>
	{
		size_t operator()(const image::IJ& ij) const noexcept
		{
			size_t seed = 0;
			boost::hash_combine(seed, ij.i);
			boost::hash_combine(seed, ij.j);
			return seed;
		}
	};
}

namespace image {

	template<typename T>
	void display(const Image<T>& image, std::ostream& out, const std::string& prefix, const std::string& suffix, const std::string& separator)
	{
		for (size_t j = 0; j < image.getHeight(); ++j) {
			out << prefix;
			out << image(0, j);
			for (size_t i = 1; i < image.getWidth(); ++i)
				out << separator << image(i, j);
			out << suffix;
		}
	}

	template<typename T, typename U, typename Function> Image<T> transformIJ(const Image<U>& inputImage, Function function)
	{
		auto outputImage = Image<T>{ inputImage.getWidth(), inputImage.getHeight() };
		for (size_t j = 0; j < inputImage.getHeight(); ++j)
			for (size_t i = 0; i < inputImage.getWidth(); ++i)
				outputImage(i, j) = function(IJ{ i, j }, inputImage(i, j));
		return outputImage;
	}

	template<typename T, typename U, typename Function> Image<T> transform(const Image<U>& inputImage, Function function)
	{
		return transformIJ<T>(inputImage, [&](const IJ&, const auto& value) { return function(value); });
	}

	template<typename T, typename U, typename V, typename Function>
	Image<T> transformIJ(const Image<U>& inputImage1, const Image<V>& inputImage2, Function function)
	{
		if (inputImage1.getWidth() != inputImage2.getWidth() || inputImage1.getHeight() != inputImage2.getHeight())
			throw std::exception{ "input maps have different sizes" };

		auto outputImage = Image<T>{ inputImage1.getWidth(), inputImage1.getHeight() };
		for (size_t j = 0; j < inputImage1.getHeight(); ++j)
			for (size_t i = 0; i < inputImage1.getWidth(); ++i)
				outputImage(i, j) = function(IJ{ i, j }, inputImage1(i, j), inputImage2(i, j));
		return outputImage;
	}

	template<typename T, typename U, typename V, typename Function>
	Image<T> transform(const Image<U>& inputImage1, const Image<V>& inputImage2, Function function)
	{
		return transformIJ<T>(inputImage1, inputImage2, [&](const IJ&, const auto& value1, const auto& value2) { return function(value1, value2); });
	}

	template<typename T> Image<T> resized(const Image<T>& image, size_t width, size_t height, T defaultValue)
	{
		auto result = Image<T>{ width, height, defaultValue };
		for (size_t j = 0; j < image.getHeight(); ++j)
			for (size_t i = 0; i < image.getWidth(); ++i)
				if (j < height && i < width)
					result(i, j) = image(i, j);
		return result;
	}

	template<typename T> void resize(Image<T>& image, size_t width, size_t height, T defaultValue)
	{ //
		image = resized(image, width, height, defaultValue);
	}
}
