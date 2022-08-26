#pragma once

#include "dnsRawResolver.hpp"

#include "SocketAddr.hpp"

namespace soup
{
	struct dnsUdpResolver : public dnsRawResolver
	{
		SocketAddr server{ IpAddr(0x01010101), 53 };
		unsigned int timeout_ms = 3000;

		[[nodiscard]] std::vector<UniquePtr<dnsRecord>> lookup(dnsType qtype, const std::string& name) const final;
	};
}
