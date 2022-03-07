#pragma once

#include "packet.hpp"

namespace soup
{
	SOUP_PACKET(tls_client_hello_ext_elliptic_curves)
	{
		std::vector<uint16_t> named_curves;

		SOUP_PACKET_IO(s)
		{
			return s.vec_u16_bl_u16(named_curves);
		}
	};
}
