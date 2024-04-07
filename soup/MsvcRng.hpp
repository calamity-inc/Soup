#pragma once

#include "RngInterface.hpp"

NAMESPACE_SOUP
{
	// Not meant for usage, just for demonstration purposes.
	struct MsvcRng
	{
		uint32_t state;

		MsvcRng(uint32_t seed) // = srand
			: state(seed)
		{
		}

		[[nodiscard]] uint32_t generate() // = rand
		{
			state *= 214013u;
			state += 2531011u;
			return ((state >> 16) & 0b111111111111111);
		}
	};
}
