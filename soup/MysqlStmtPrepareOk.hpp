#pragma once

#include "Packet.hpp"

NAMESPACE_SOUP
{
	SOUP_PACKET(MysqlStmtPrepareOk)
	{
		u32 stmt_id;
		u16 num_columns;
		u16 num_params;
		u16 num_warnings;

		SOUP_PACKET_IO(s)
		{
			return s.u32_le(stmt_id)
				&& s.u16_le(num_columns)
				&& s.u16_le(num_params)
				&& s.skip(1)
				&& s.u16_le(num_warnings)
				;
		}
	};
}
