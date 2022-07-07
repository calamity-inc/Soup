#pragma once

#include <cstdint>

namespace soup
{
	// Based on https://www.dlbeer.co.nz/oss/c25519.html and https://github.com/DavyLandman/compact25519

	struct ec
	{
		static constexpr auto X25519_KEY_SIZE = 32;
		static constexpr auto X25519_SHARED_SIZE = 32;

		static void curve25519_generatePrivate(uint8_t(&private_key)[X25519_KEY_SIZE]);
		static void curve25519_derivePublic(uint8_t* public_key, const uint8_t* private_key);

		static void x25519(uint8_t(&shared_secret)[X25519_SHARED_SIZE], const uint8_t(&my_private_key)[X25519_KEY_SIZE], const uint8_t(&their_public_key)[X25519_KEY_SIZE]);
	};
}
