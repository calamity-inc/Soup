#include "base.hpp"

#include "Exception.hpp"

namespace soup
{
	void throwAssertionFailed()
	{
		throw Exception("Assertion failed");
	}
}
