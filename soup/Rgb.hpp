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
		static Rgb GREEN;
		static Rgb BLUE;

		[[nodiscard]] bool operator==(const Rgb& c) const noexcept;
		[[nodiscard]] bool operator!=(const Rgb& c) const noexcept;

		[[nodiscard]] std::string toHex() const;
	};
	static_assert(sizeof(Rgb) == 3);
#pragma pack(pop)
}
