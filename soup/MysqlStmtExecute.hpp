#pragma once

NAMESPACE_SOUP
{
	SOUP_PACKET(MysqlStmtExecute)
	{
		u32 stmt_id;
		u8 flags;
		u32 iterations;

		SOUP_PACKET_IO(s)
		{
			return s.u32_le(stmt_id)
				&& s.u8(flags)
				&& s.u32_le(iterations)
				;
		}
	};
}
