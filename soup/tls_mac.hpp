#pragma once

#include "tls_record.hpp"

namespace soup
{
	SOUP_PACKET(tls_mac)
	{
		u64 seq_num;
		tls_record record{};

		SOUP_PACKET_IO(s)
		{
			return s.u64(seq_num)
				&& record.io(s)
				;
		}
	};
}
