#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "dnsResolver.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct netConfig
	{
		[[nodiscard]] static netConfig& get(); // returns the netConfig instance for this thread

		UniquePtr<dnsResolver> dns_resolver;

		netConfig();
	};
}

#endif
