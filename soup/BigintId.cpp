#include "BigintId.hpp"

#include "LcgRngInterface.hpp"
#include "Bigint.hpp"

namespace soup
{
	BigintId BigintId::forRandomProbablePrime(const size_t bits, const int miller_rabin_iterations)
	{
		LcgRngInterface rng;
		Bigint i;
		while (true)
		{
			const uint64_t seed = rng.getState();
			i = Bigint::random(rng, bits);
			i.enableBitInbounds(0);
			if (i.isProbablePrime(miller_rabin_iterations))
			{
				return BigintId{ seed };
			}
		}
	}

	Bigint BigintId::getBigint(const size_t bits) const
	{
		LcgRngInterface rng(seed);
		return Bigint::random(rng, bits);
	}
}
