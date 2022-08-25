#pragma once

#include <cstdint>

namespace soup
{
	enum dnsType : uint16_t
	{
#include "dnsTypeImpl.hpp"
	};
}
