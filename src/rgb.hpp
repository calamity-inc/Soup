#pragma once

#include <cstdint>

namespace soup
{
#pragma pack(push, 1)
	struct rgb
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;

		[[nodiscard]] bool isPureBlack() const noexcept
		{
			return r == 0 && g == 0 && b == 0;
		}
	};
	static_assert(sizeof(rgb) == 3);
#pragma pack(pop)
}
