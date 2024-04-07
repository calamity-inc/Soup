#pragma once

#include "Rgb.hpp"

NAMESPACE_SOUP
{
	union Rgba
	{
		struct
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
		uint8_t arr[4];
		Rgb rgb;

		constexpr Rgba() noexcept
			: r(0), g(0), b(0), a(0)
		{
		}

		template <typename TR, typename TG, typename TB, typename TA>
		constexpr Rgba(TR r, TG g, TB b, TA a) noexcept
			: r((uint8_t)r), g((uint8_t)g), b((uint8_t)b), a((uint8_t)a)
		{
		}

		constexpr Rgba(uint32_t val) noexcept
			: r(val >> 24), g((val >> 16) & 0xFF), b((val >> 8) & 0xFF), a(val & 0xFF)
		{
		}

		[[nodiscard]] constexpr bool operator==(const Rgba& c) const noexcept
		{
			return r == c.r && g == c.g && b == c.b;
		}

		[[nodiscard]] constexpr bool operator!=(const Rgba& c) const noexcept
		{
			return !operator==(c);
		}

		[[nodiscard]] static Rgba fromHex(std::string hex);

		[[nodiscard]] constexpr uint32_t toInt() const noexcept
		{
			return (r << 24) | (g << 16) | (b << 8) | a;
		}

		[[nodiscard]] uint8_t& operator[](unsigned int i)
		{
			return arr[i];
		}

		[[nodiscard]] const uint8_t& operator[](unsigned int i) const
		{
			return arr[i];
		}
	};
}
