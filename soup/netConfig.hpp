#pragma once

#include "fwd.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct netConfig
	{
		[[nodiscard]] static netConfig& get(); // returns the netConfig instance for this thread

		UniquePtr<dnsResolver> dns_resolver;
		UniquePtr<dnsHttpResolver> schedulable_dns_resolver; // at some point in the future, makeLookupTask should be added to all resolvers, but it might be a bit tricky with OS resolvers since on Linux, the OS resolver is blocking & not thread-safe. Windows should be fine since it has an async API.

		netConfig();
	};
}
