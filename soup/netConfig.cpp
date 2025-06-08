#include "netConfig.hpp"

NAMESPACE_SOUP
{
	static thread_local netConfig netConfig_inst;

	netConfig& netConfig::get()
	{
		return netConfig_inst;
	}

	dnsResolver& netConfig::getDnsResolver() SOUP_EXCAL
	{
		if (!dns_resolver)
		{
			dns_resolver = dnsResolver::makeDefault();
		}
		return *dns_resolver;
	}

	netConfig::netConfig()
	{
	}
}
