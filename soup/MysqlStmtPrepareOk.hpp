#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(MysqlStmtPrepareOk)
	{
		u32 stmt_id;
		u16 num_columns;
		u16 num_params;
		u16 num_warnings;

		SOUP_PACKET_IO(s)
		{
			return s.u32(stmt_id)
				&& s.u16(num_columns)
				&& s.u16(num_params)
				&& s.skip(1)
				&& s.u16(num_warnings)
				;
		}
	};
}
