#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "dnsRawResolver.hpp"

#include "SocketAddr.hpp"

namespace soup
{
	struct dnsUdpResolver : public dnsRawResolver
	{
		SocketAddr server{ IpAddr(native_u32_t(0x01010101)), native_u16_t(53) };
		unsigned int timeout_ms = 3000;

		[[nodiscard]] std::vector<UniquePtr<dnsRecord>> lookup(dnsType qtype, const std::string& name) const final;
	};
}

#endif
