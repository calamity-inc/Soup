#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(TlsEncryptedPreMasterSecret)
	{
		std::string data;

		SOUP_PACKET_IO(s)
		{
			return s.str_lp_u16(data);
		}
	};
}
