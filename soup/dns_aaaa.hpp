#pragma once

#include "dns.hpp"

#include "addr_ip.hpp"

namespace soup
{
	struct dns_aaaa : public dns
	{
		addr_ip data;

		dns_aaaa(uint32_t ttl, const uint8_t* data)
			: dns(ttl), data(data)
		{
		}
	};
}
