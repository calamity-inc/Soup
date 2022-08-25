#pragma once

#include <cstdint>

namespace soup
{
	enum dnsQType : uint16_t
	{
#include "dnsTypeImpl.hpp"
		DNS_AXFR = 252, // A request for a transfer of an entire zone
		DNS_MAILB = 253, // A request for mailbox-related records (MB, MG or MR)
		DNS_MAILA = 254, // A request for mail agent RRs (Obsolete - see MX)
		DNS_WILDCARD = 255, // A request for all records
	};
}
