#include "LcgRng.hpp"

#include "rand.hpp"

namespace soup
{
	LcgRng::LcgRng()
		: LcgRng(rand.getSeed())
	{
	}
}
