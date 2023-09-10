#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "type.hpp"

#include "dnsResolver.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct netConfig
	{
		[[nodiscard]] static netConfig& get(); // returns the netConfig instance for this thread

		UniquePtr<dnsResolver> dns_resolver;
		bool prefer_ipv6 = false; // for funny things like https://api.lovense.com/api/lan/getToys, currently not respected by netConnectTask

		certchain_validator_t certchain_validator;

		netConfig();
	};
}

#endif
