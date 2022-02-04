#pragma once

#include "dns.hpp"

#include <string>

namespace soup
{
	struct dns_txt : public dns
	{
		std::string data;

		dns_txt(uint32_t ttl, std::string&& data)
			: dns(ttl), data(std::move(data))
		{
		}
	};
}
