#include "LcgRng.hpp"

#include "rand.hpp"

NAMESPACE_SOUP
{
	LcgRng::LcgRng()
		: LcgRng(rand.getSeed())
	{
	}
}
