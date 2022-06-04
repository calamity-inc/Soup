#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(MysqlHandshake)
	{
		u8 protocol_version;
		std::string human_readable_version;
		u32 thread_id;
		std::string salt_pt_1;
		u16 capabilities_lo;
		u8 language;
		u16 status;
		u16 capabilities_hi;
		u8 auth_plugin_data_len;
		u32 mariadb_capabilities;
		std::string salt_pt_2;
		std::string auth_plugin_name;

		SOUP_PACKET_IO(s)
		{
			return s.u8(protocol_version)
				&& s.str_nt(human_readable_version)
				&& s.u32(thread_id)
				&& s.str(8, salt_pt_1)
				&& s.ignore(1)
				&& s.u16(capabilities_lo)
				&& s.u8(language)
				&& s.u16(status)
				&& s.u16(capabilities_hi)
				&& s.u8(auth_plugin_data_len)
				&& s.ignore(6)
				&& s.u32(mariadb_capabilities)
				&& s.str(12, salt_pt_2)
				&& s.ignore(1)
				&& s.str_nt(auth_plugin_name)
				;
		}
	};
}
