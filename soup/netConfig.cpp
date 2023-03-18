#include "netConfig.hpp"

#include "dnsOsResolver.hpp"

namespace soup
{
	static thread_local netConfig inst;

	netConfig& netConfig::get()
	{
		return inst;
	}

	netConfig::netConfig()
		: dns_resolver(soup::make_unique<dnsOsResolver>())
	{
	}
}
