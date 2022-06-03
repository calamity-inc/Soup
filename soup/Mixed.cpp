#include "Mixed.hpp"

namespace soup
{
	intptr_t Mixed::getInt() const
	{
		if (type != INT)
		{
			throw 0;
		}
		return static_cast<intptr_t>(val);
	}

	uintptr_t Mixed::getUInt() const
	{
		if (type != UINT)
		{
			throw 0;
		}
		return val;
	}

	const std::string& Mixed::getString() const
	{
		if (type != STRING)
		{
			throw 0;
		}
		return *reinterpret_cast<std::string*>(val);
	}
}
