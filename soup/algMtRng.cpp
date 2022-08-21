#include "algMtRng.hpp"

#include "rand.hpp"

namespace soup
{
	uint64_t algMtRng::generate()
	{
		return rand.t<uint64_t>(0, -1);
	}
}
