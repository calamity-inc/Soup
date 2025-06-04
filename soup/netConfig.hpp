#pragma once

#include "base.hpp"
#include "type.hpp"

#include "dnsResolver.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	struct netConfig
	{
		[[nodiscard]] static netConfig& get(); // returns the netConfig instance for this thread

		UniquePtr<dnsResolver> dns_resolver;

		[[nodiscard]] dnsResolver& getDnsResolver() SOUP_EXCAL;

		netConfig();
	};
}
