#pragma once

#include <random>

#include "macros.hpp"

#define ARRAY_RAND(arr) arr[::soup::rand(0, COUNT(arr) - 1)]

namespace soup
{
	struct rand_impl
	{
		template <typename T>
		[[nodiscard]] static T t(T min, T max) noexcept
		{
			std::random_device rd{};
			std::mt19937_64 gen{ rd() };
			std::uniform_int_distribution<T> distr{ min, max };
			return distr(gen);
		}

		[[nodiscard]] size_t operator()(size_t min, size_t max) const noexcept
		{
			return t<size_t>(min, max);
		}

		template <typename T>
		[[nodiscard]] T& operator()(std::vector<T>& vec) const
		{
			return vec.at(t<size_t>(0, vec.size() - 1));
		}

		template <typename T>
		[[nodiscard]] T operator()(std::vector<T>&& vec) const
		{
			return std::move(vec.at(t<size_t>(0, vec.size() - 1)));
		}

		[[nodiscard]] static uint8_t byte() noexcept
		{
			return static_cast<uint8_t>(t<unsigned short>(0u, 0xFFu));
		}

		[[nodiscard]] static char ch() noexcept
		{
			return (char)byte();
		}

		[[nodiscard]] static bool coinflip() noexcept
		{
			return t(0, 1) != 0;
		}

		template <typename T>
		[[nodiscard]] static bool one_in(T odds) noexcept
		{
			return t<T>(0, odds - 1) == 0;
		}

		template <typename StrT, typename IntT>
		[[nodiscard]] static StrT str(const IntT len)
		{
			StrT str{};
			static const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
			for (IntT i = 0; i < len; i++)
			{
				str.push_back(chars[t<IntT>(0, COUNT(chars) - 2)]);
			}
			return str;
		}
	};

	inline rand_impl rand;
}
