#pragma once

#include "packet.hpp"

namespace soup
{
	SOUP_PACKET(tls_encrypted_pre_master_secret)
	{
		std::string data;

		SOUP_PACKET_IO(s)
		{
			return s.str_lp_u16(data);
		}
	};
}
