#pragma once

#include "dns.hpp"

namespace soup
{
	struct dns_a : public dns
	{
		uint32_t data;

		constexpr dns_a(uint32_t ttl, uint32_t data)
			: dns(ttl), data(data)
		{
		}
	};
}
