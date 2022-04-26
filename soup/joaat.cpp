#include "joaat.hpp"

namespace soup
{
	uint32_t joaat::hash(const std::string& str) noexcept
	{
		uint32_t val = 0;
		for (const auto& c : str)
		{
			val += (uint8_t)c;
			val += (val << 10);
			val ^= (val >> 6);
		}
		val += (val << 3);
		val ^= (val >> 11);
		val += (val << 15);
		return val;
	}
}
