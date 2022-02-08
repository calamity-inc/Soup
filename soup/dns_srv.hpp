#pragma once

#include "dns.hpp"

#include <string>

namespace soup
{
	struct dns_srv : public dns
	{
		uint16_t priority;
		uint16_t weight;
		std::string target;
		uint16_t port;

		dns_srv(uint32_t ttl, uint16_t priority, uint16_t weight, std::string&& target, uint16_t port)
			: dns(ttl), priority(priority), weight(weight), target(std::move(target)), port(port)
		{
		}
	};
}
