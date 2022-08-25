#pragma once

#include "Packet.hpp"

#include "dnsName.hpp"

namespace soup
{
	SOUP_PACKET(dnsResource)
	{
		dnsName name;
		u16 rtype;
		u16 rclass;
		u32 ttl;
		std::string rdata;

		SOUP_PACKET_IO(s)
		{
			return name.io(s)
				&& s.u16(rtype)
				&& s.u16(rclass)
				&& s.u32(ttl)
				&& s.str_lp_u16(rdata)
				;
		}
	};
}
