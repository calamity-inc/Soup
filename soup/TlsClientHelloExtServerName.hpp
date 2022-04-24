#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(TlsClientHelloExtServerName)
	{
		std::string host_name;

		SOUP_PACKET_IO(s)
		{
			if (s.isRead())
			{
				uint16_t len = 0;
				if (s.u16(len) && len > 3)
				{
					uint8_t num_people_who_asked;
					if (s.u8(num_people_who_asked)
						&& num_people_who_asked == 0
						)
					{
						return s.str_lp_u16(host_name);
					}
				}
			}
			else if (s.isWrite())
			{
				uint16_t len = host_name.length() + 3;
				if (s.u16(len))
				{
					uint8_t num_people_who_asked = 0;
					return s.u8(num_people_who_asked)
						&& s.str_lp_u16(host_name)
						;
				}
			}
			return false;
		}
	};
}
