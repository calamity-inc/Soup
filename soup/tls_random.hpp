#pragma once

#include "packet.hpp"

namespace soup
{
	SOUP_PACKET(tls_random)
	{
		u32 time;
		u8 random[28];

		SOUP_PACKET_IO(s)
		{
			if (!s.u32(time))
			{
				return false;
			}
			for (auto& b : random)
			{
				if (!s.u8(b))
				{
					return false;
				}
			}
			return true;
		}
	};
	static_assert(sizeof(tls_random) == 32);
}
