#pragma once

#include "algRng.hpp"

namespace soup
{
	// Not meant for usage, just for demonstration purposes.
	struct algMsvcRng : public algRng
	{
		uint32_t state;

		algMsvcRng(uint32_t seed) // = srand
			: state(seed)
		{
		}

		[[nodiscard]] uint64_t generate() final // = rand
		{
			state *= 214013u;
			state += 2531011u;
			return ((state >> 16) & 0b111111111111111);
		}
	};
}
