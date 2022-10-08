#pragma once

#include <cstdint>

namespace soup
{
	// Linear congruential generator (LCG)
	struct LcgRng
	{
		uint64_t state;
		// modulus 2^64 (aka. overflowing a 64-bit integer)
		uint64_t multiplier = 6364136223846793005ull;
		uint64_t increment = 1442695040888963407ull;

		LcgRng();

		LcgRng(uint64_t seed)
			: state(seed)
		{
		}

		[[nodiscard]] uint64_t generate();
	};
}
