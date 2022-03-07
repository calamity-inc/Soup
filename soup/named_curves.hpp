#pragma once

#include "scoped_enum.hpp"

namespace soup
{
	SCOPED_ENUM(named_curves, uint16_t,
		secp256k1 = 22,
		secp256r1 = 23,
		brainpoolP256r1 = 26,
		x25519 = 29,
		brainpoolP256r1tls13 = 31,
	);
}
