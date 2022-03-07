#pragma once

#include "type.hpp"

#include "packet.hpp"

#include "tls_hello_extensions.hpp"
#include "tls_protocol_version.hpp"
#include "tls_random.hpp"

namespace soup
{
	SOUP_PACKET(tls_client_hello)
	{
		tls_protocol_version version{};
		tls_random random;
		std::string session_id{};
		std::vector<tls_cipher_suite_t> cipher_suites{};
		std::vector<uint8_t> compression_methods{};
		tls_hello_extensions extensions{};

		SOUP_PACKET_IO(s)
		{
			return version.io(s)
				&& random.io(s)
				&& s.str_lp_u8(session_id, 32)
				&& s.vec_u16_bl_u16(cipher_suites)
				&& s.vec_u8_u8(compression_methods)
				&& extensions.io(s)
				;
		}
	};
}
