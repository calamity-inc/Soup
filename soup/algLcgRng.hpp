#pragma once

#include "algRng.hpp"

namespace soup
{
	// Linear congruential generator (LCG)
	struct algLcgRng : public algRng
	{
		uint64_t state;
		// modulus 2^64 (aka. overflowing a 64-bit integer)
		uint64_t multiplier = 6364136223846793005ull;
		uint64_t increment = 1442695040888963407ull;

		algLcgRng();

		algLcgRng(uint64_t seed)
			: state(seed)
		{
		}

		[[nodiscard]] uint64_t generate() final;
	};
}
