#pragma once

#include "packet.hpp"

namespace soup
{
	SOUP_PACKET(tls_server_key_exchange)
	{
		u8 curve_type;
		u16 named_curve;
		std::string point;

		SOUP_PACKET_IO(s)
		{
			return s.u8(curve_type)
				&& curve_type == 3
				&& s.u16(named_curve)
				&& s.str_lp_u8(point)
				;
		}
	};
}
