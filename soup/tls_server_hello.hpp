#pragma once

#include "type.hpp"

#include "packet.hpp"

#include "tls_hello_extensions.hpp"
#include "tls_protocol_version.hpp"
#include "tls_random.hpp"

namespace soup
{
	SOUP_PACKET(tls_server_hello)
	{
		tls_protocol_version version{};
		tls_random random;
		std::string session_id{};
		tls_cipher_suite_t cipher_suite;
		u8 compression_method;
		tls_hello_extensions extensions{};

		SOUP_PACKET_IO(s)
		{
			return version.io(s)
				&& random.io(s)
				&& s.str_lp_u8(session_id, 32)
				&& s.u16(cipher_suite)
				&& s.u8(compression_method)
				&& extensions.io(s)
				;
		}
	};
}
