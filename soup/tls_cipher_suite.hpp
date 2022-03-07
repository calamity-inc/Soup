#pragma once

#include "type.hpp"

namespace soup
{
	enum tls_cipher_suite : tls_cipher_suite_t
	{
		TLS_RSA_WITH_AES_128_CBC_SHA = 0x002F,
		TLS_RSA_WITH_AES_256_CBC_SHA = 0x0035,
		TLS_RSA_WITH_AES_128_CBC_SHA256 = 0x003C,
		TLS_RSA_WITH_AES_256_CBC_SHA256 = 0x003D,

		TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA = 0xC009,
		TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA = 0xC00A,
		TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 = 0xC023,
	};
}
