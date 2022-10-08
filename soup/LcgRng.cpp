#include "LcgRng.hpp"

#include "Endian.hpp"
#include "rand.hpp"

namespace soup
{
	LcgRng::LcgRng()
		: LcgRng(rand.getSeed())
	{
	}

	uint64_t LcgRng::generate()
	{
		state *= multiplier;
		state += increment;

		// invert byte order since the higher-order bits have longer periods
		return Endianness::invert(state);
	}
}
