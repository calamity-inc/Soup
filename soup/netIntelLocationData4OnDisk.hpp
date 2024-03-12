#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(netIntelLocationData4OnDisk)
	{
		uint32_t lower;
		uint32_t upper;

		std::string country_code;
		uint32_t state_offset;
		uint32_t city_offset;

		SOUP_PACKET_IO(s)
		{
			return s.u32(lower)
				&& s.u32(upper)
				&& s.str(2, country_code)
				&& s.u32(state_offset)
				&& s.u32(city_offset)
				;
		}

#if SOUP_CPP20
		[[nodiscard]] static std::strong_ordering cmp(Reader & r, const uint32_t & hint)
		{
			netIntelLocationData4OnDisk data;
			r.u32(data.lower);
			r.u32(data.upper);
			if (data.lower <= hint && hint <= data.upper)
			{
				return std::strong_ordering::equal;
			}
			return data.lower < hint ? std::strong_ordering::greater : std::strong_ordering::less;
		}
#endif
	};
}
