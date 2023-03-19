#include "netConfig.hpp"

#if !SOUP_WASM

#include "dnsHttpResolver.hpp"
#include "dnsOsResolver.hpp"

namespace soup
{
	static thread_local netConfig inst;

	netConfig& netConfig::get()
	{
		return inst;
	}

	netConfig::netConfig() :
		dns_resolver(soup::make_unique<dnsOsResolver>()),
		schedulable_dns_resolver(soup::make_unique<dnsHttpResolver>())
	{
	}
}

#endif
