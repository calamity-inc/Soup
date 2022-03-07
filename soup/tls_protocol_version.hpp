#pragma once

#include "packet.hpp"

namespace soup
{
	struct tls_protocol_version
	{
		uint8_t major = 3;
		uint8_t minor = 3;

		SOUP_PACKET_IO(s)
		{
			return s.u8(major)
				&& s.u8(minor)
				;
		}
	};
}
