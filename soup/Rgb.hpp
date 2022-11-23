#pragma once

#include <cstdint>
#include <string>

#include "math.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct Rgb
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;

		static const Rgb BLACK;
		static const Rgb WHITE;
		static const Rgb RED;
		static const Rgb YELLOW;
		static const Rgb GREEN;
		static const Rgb BLUE;
		static const Rgb MAGENTA;
		static const Rgb GRAY;

		constexpr Rgb() noexcept = default;

		template <typename TR, typename TG, typename TB>
		constexpr Rgb(TR r, TG g, TB b) noexcept
			: r((uint8_t)r), g((uint8_t)g), b((uint8_t)b)
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
				soup::lerp<uint8_t>(a.r, b.r, t),
				soup::lerp<uint8_t>(a.g, b.g, t),
				soup::lerp<uint8_t>(a.b, b.b, t),
			};
		}
	};
	static_assert(sizeof(Rgb) == 3);
#pragma pack(pop)
}
