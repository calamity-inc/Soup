#pragma once

#include "base.hpp"

#if SOUP_CPP20
#include <bit>
#endif

NAMESPACE_SOUP
{
#if !SOUP_CPP20
	template <typename T>
	[[nodiscard]] constexpr T rotr(const T val, int shift);
#endif

	template <typename T>
	[[nodiscard]] constexpr T rotl(const T val, int shift)
	{
#if SOUP_CPP20
		return std::rotl<T>(val, shift);
#else
		const auto bits = sizeof(T) * 8;
		shift %= bits;
		if (shift > 0)
		{
			return (val << shift) | (val >> (bits - shift));
		}
		else if (shift == 0)
		{
			return val;
		}
		else // if (shift < 0)
		{
			return rotr(val, -shift);
		}
#endif
	}

	template <typename T>
	[[nodiscard]] constexpr T rotr(const T val, int shift)
	{
#if SOUP_CPP20
		return std::rotr<T>(val, shift);
#else
		const auto bits = sizeof(T) * 8;
		shift %= bits;
		if (shift > 0)
		{
			return (val >> shift) | (val << (bits - shift));
		}
		else if (shift == 0)
		{
			return val;
		}
		else // if (shift < 0)
		{
			return rotl(val, -shift);
		}
#endif
	}
}
