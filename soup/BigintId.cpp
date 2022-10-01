#include "BigintId.hpp"

#include "algLcgRng.hpp"
#include "Bigint.hpp"

namespace soup
{
	BigintId BigintId::forRandomProbablePrime(const size_t bits, const int miller_rabin_iterations)
	{
		algLcgRng rng;
		Bigint i;
		while (true)
		{
			const uint64_t seed = rng.state;
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
		algLcgRng rng(seed);
		return Bigint::random(rng, bits);
	}
}
