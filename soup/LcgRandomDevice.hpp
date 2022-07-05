#pragma once

#include "RandomDevice.hpp"

namespace soup
{
	// Linear congruential generator (LCG)
	struct LcgRandomDevice : public RandomDevice
	{
		uint64_t last;
		// modulus 2^64 (aka. overflowing a 64-bit integer)
		uint64_t multiplier = 6364136223846793005ull;
		uint64_t increment = 1442695040888963407ull;

		LcgRandomDevice();

		LcgRandomDevice(uint64_t seed)
			: last(seed)
		{
		}

		[[nodiscard]] uint64_t generate() final;
	};
}
