#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(MysqlFrame)
	{
		u24 length;
		u8 seq_id;

		SOUP_PACKET_IO(s)
		{
			return s.u24(length)
				&& s.u8(seq_id)
				;
		}
	};
}
