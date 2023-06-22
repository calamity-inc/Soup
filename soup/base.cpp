#include "base.hpp"

#include "Exception.hpp"
#include "ObfusString.hpp"

namespace soup
{
	void throwAssertionFailed()
	{
		throw Exception(ObfusString("Assertion failed").str());
	}
}
