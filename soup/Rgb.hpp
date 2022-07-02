#pragma once

#include <cstdint>
#include <string>

namespace soup
{
#pragma pack(push, 1)
	struct Rgb
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;

		static Rgb BLACK;
		static Rgb WHITE;
		static Rgb RED;
		static Rgb YELLOW;
		static Rgb GREEN;
		static Rgb BLUE;

		constexpr Rgb() noexcept = default;

		constexpr Rgb(uint8_t r, uint8_t g, uint8_t b) noexcept
			: r(r), g(g), b(b)
		{
		}

		constexpr Rgb(uint32_t val) noexcept
			: r(val >> 16), g((val >> 8) & 0xFF), b(val & 0xFF)
		{
		}

		[[nodiscard]] constexpr bool operator==(const Rgb& c) const noexcept
		{
			return r == c.r && g == c.g && b == c.b;
		}

		[[nodiscard]] constexpr bool operator!=(const Rgb& c) const noexcept
		{
			return !operator==(c);
		}

		[[nodiscard]] constexpr uint32_t toInt() const noexcept
		{
			return (r << 16) | (g << 8) | b;
		}

		[[nodiscard]] std::string toHex() const;

		[[nodiscard]] static Rgb lerp(Rgb a, Rgb b, float t)
		{
			return Rgb{
				uint8_t(a.r + (b.r - a.r) * t),
				uint8_t(a.g + (b.g - a.g) * t),
				uint8_t(a.b + (b.b - a.b) * t),
			};
		}
	};
	static_assert(sizeof(Rgb) == 3);
#pragma pack(pop)
}
