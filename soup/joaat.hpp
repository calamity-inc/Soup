#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct joaat
	{
		[[nodiscard]] static uint32_t hash(const std::string& str, uint32_t initial = 0) noexcept;

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

		[[nodiscard]] static constexpr uint32_t hash(const char* str, uint32_t initial) noexcept
		{
			uint32_t val = initial;
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

		[[nodiscard]] static constexpr uint32_t partial(const char* data, size_t size, uint32_t initial = 0) noexcept
		{
			/*uint32_t val = initial;
			while (size-- != 0)
			{
				val += *(data++);
				val += (val << 10);
				val ^= (val >> 6);
			}
			return val;*/

			unsigned __int64 v3 = 0;
			uint32_t result;
			int v5;
			for (result = initial; v3 < size; result = ((uint32_t)(1025 * (v5 + result)) >> 6) ^ (1025 * (v5 + result)))
			{
				v5 = data[v3++];
			}
			return result;
		}

		static void finalise(uint32_t& val) noexcept
		{
			/*val += (val << 3);
			val ^= (val >> 11);
			val += (val << 15);*/

			val = (0x8001 * (((uint32_t)(9 * val) >> 11) ^ (9 * val)));
		}

		static void undo_finalise(uint32_t& val) noexcept
		{
			val *= 0x3FFF8001; // inverse of val += (val << 15);
			val ^= (val >> 11) ^ (val >> 22);
			val *= 0x38E38E39; // inverse of val += (val << 3);
		}
	};
}
