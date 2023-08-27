#include "base.hpp"

#include "Exception.hpp"
#include "ObfusString.hpp"

namespace soup
{
	void throwAssertionFailed()
	{
		throw Exception(ObfusString("Assertion failed").str());
	}

	void throwAssertionFailed(const char* what)
	{
		std::string msg = "Assertion failed: ";
		msg.append(what);
		throw Exception(std::move(msg));
	}
}
