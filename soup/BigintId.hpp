#pragma once

#include <cstdint>

#include "Bigint.hpp"

namespace soup
{
	struct BigintId
	{
		uint64_t seed;

		[[nodiscard]] static BigintId forRandomProbablePrime(const size_t bits, const int miller_rabin_iterations = 1);

		[[nodiscard]] Bigint getBigint(const size_t bits) const;
	};
}
