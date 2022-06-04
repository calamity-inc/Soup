#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(MysqlAuthSwitchRequest) // first byte = 0xFE
	{
		std::string plugin_name;
		std::string plugin_data;

		SOUP_PACKET_IO(s)
		{
			return s.str_nt(plugin_name)
				&& s.str_nt(plugin_data)
				;
		}
	};
}
