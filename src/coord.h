#pragma once

#include <array>
#include <boost/functional/hash.hpp>

namespace coord {

	template<size_t Dim> class Coord
	{
	public:
		Coord() = default;
		Coord(std::array<std::int64_t, Dim> v) : v_{ std::move(v) } {}

		std::int64_t  operator[](size_t i) const { return v_[i]; }
		std::int64_t& operator[](size_t i) { return v_[i]; }

		friend bool operator==(const Coord& lhs, const Coord& rhs) { return lhs.v_ == rhs.v_; }
		friend bool operator!=(const Coord& lhs, const Coord& rhs) { return lhs.v_ != rhs.v_; }
		friend bool operator<(const Coord& lhs, const Coord& rhs) { return lhs.v_ < rhs.v_; }
		friend bool operator>(const Coord& lhs, const Coord& rhs) { return lhs.v_ > rhs.v_; }
		friend bool operator<=(const Coord& lhs, const Coord& rhs) { return lhs.v_ <= rhs.v_; }
		friend bool operator>=(const Coord& lhs, const Coord& rhs) { return lhs.v_ >= rhs.v_; }

		Coord& operator+=(const Coord& rhs)
		{
			for (size_t i = 0; i < Dim; ++i)
				v_[i] += rhs.v_[i];
			return *this;
		}

		Coord& operator-=(const Coord& rhs)
		{
			for (size_t i = 0; i < Dim; ++i)
				v_[i] -= rhs.v_[i];
			return *this;
		}

		friend Coord operator+(Coord lhs, const Coord& rhs) { return lhs += rhs; }
		friend Coord operator-(Coord lhs, const Coord& rhs) { return lhs -= rhs; }

	private:
		std::array<std::int64_t, Dim> v_{};
	};

	template<size_t Dim> Coord<Dim> abs(Coord<Dim> c)
	{
		for (size_t i = 0; i < Dim; ++i)
			c[i] = std::abs(c[i]);
		return c;
	}
}

namespace std {

	template<size_t Dim> struct hash<coord::Coord<Dim>>
	{
		size_t operator()(const coord::Coord<Dim>& c) const noexcept
		{
			size_t seed = 0;
			for (size_t i = 0; i < Dim; ++i)
				boost::hash_combine(seed, c[i]);
			return seed;
		}
	};
}