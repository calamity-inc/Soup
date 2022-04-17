#pragma once

#include <cstdint>
#include <string>

namespace soup
{
#pragma pack(push, 1)
	struct rgb
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;

		static rgb BLACK;
		static rgb WHITE;

		[[nodiscard]] bool operator==(const rgb& c) const noexcept;
		[[nodiscard]] bool operator!=(const rgb& c) const noexcept;

		[[nodiscard]] std::string toHex() const;
	};
	static_assert(sizeof(rgb) == 3);
#pragma pack(pop)
}
