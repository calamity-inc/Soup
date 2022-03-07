#pragma once

#include "packet.hpp"

namespace soup
{
	SOUP_PACKET(tls_certificate)
	{
		std::vector<std::string> asn1_certs{};

		SOUP_PACKET_IO(s)
		{
			return s.vec_str_lp_u24_bl_u24(asn1_certs);
		}
	};
}
