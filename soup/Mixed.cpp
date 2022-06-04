#include "Mixed.hpp"

#include <ostream>

namespace soup
{
	int64_t Mixed::getInt() const
	{
		if (type != INT)
		{
			throw 0;
		}
		return (int64_t)val;
	}

	uint64_t Mixed::getUInt() const
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

	std::ostream& operator<<(std::ostream& os, const Mixed& v)
	{
		os << v.toString();
		return os;
	}
}
