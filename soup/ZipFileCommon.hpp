#pragma once

#include "Packet.hpp"

NAMESPACE_SOUP
{
	SOUP_PACKET(ZipFileCommon)
	{
		u16 min_extract_version;
		u16 bitflag;
		u16 compression_method;
		u16 last_mod_time;
		u16 last_mod_date;
		u32 uncompressed_data_crc32;
		u32 compressed_size;
		u32 uncompressed_size;

		SOUP_PACKET_IO(s)
		{
			return s.u16(min_extract_version)
				&& s.u16(bitflag)
				&& s.u16(compression_method)
				&& s.u16(last_mod_time)
				&& s.u16(last_mod_date)
				&& s.u32(uncompressed_data_crc32)
				&& s.u32(compressed_size)
				&& s.u32(uncompressed_size)
				;
		}
	};
}
