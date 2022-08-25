#include "algLcgRng.hpp"

#include "intutil.hpp"
#include "time.hpp"

namespace soup
{
	algLcgRng::algLcgRng()
		: algLcgRng(~time::nanos())
	{
	}

	uint64_t algLcgRng::generate()
	{
		state *= multiplier;
		state += increment;

		// invert byte order since the higher-order bits have longer periods
		return intutil::invertEndianness(state);
	}
}