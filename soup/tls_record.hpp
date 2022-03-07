#pragma once

#include "packet.hpp"

#include "tls_content_type.hpp"
#include "tls_protocol_version.hpp"

namespace soup
{
	SOUP_PACKET(tls_record)
	{
		tls_content_type_t content_type;
		tls_protocol_version version{};
		uint16_t length;

		SOUP_PACKET_IO(s)
		{
			return s.u8(content_type)
				&& version.io(s)
				&& s.u16(length)
				;
		}
	};
}
