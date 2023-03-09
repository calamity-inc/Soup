#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "fwd.hpp"

#include <string>

namespace soup
{
	class netInfo
	{
	public:
		// Requests the device's internet address as seen by another device, or [::] if unavailable.
		[[nodiscard]] static IpAddr getPublicAddress();
		[[nodiscard]] static IpAddr getPublicAddressV4();
		[[nodiscard]] static IpAddr getPublicAddressV6();
	protected:
		[[nodiscard]] static IpAddr getPublicAddressImpl(const std::string& provider);
	};
}

#endif
