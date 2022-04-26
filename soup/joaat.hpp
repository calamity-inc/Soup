#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct joaat
	{
		[[nodiscard]] static uint32_t hash(const std::string& str) noexcept;

		[[nodiscard]] static constexpr uint32_t hash(const char* str) noexcept
		{
			uint32_t val = 0;
			while (*str)
			{
				val += (uint8_t)*(str++);
				val += (val << 10);
				val ^= (val >> 6);
			}
			val += (val << 3);
			val ^= (val >> 11);
			val += (val << 15);
			return val;
		}
	};
}
