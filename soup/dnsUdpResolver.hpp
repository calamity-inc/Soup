#pragma once

#include "dnsRawResolver.hpp"

#include "SocketAddr.hpp"

namespace soup
{
	struct dnsUdpResolver : public dnsRawResolver
	{
		SocketAddr server{ IpAddr(0x01010101), 53 };
		unsigned int timeout_ms = 3000;

		[[nodiscard]] std::vector<dnsARecord> lookupA(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsAaaaRecord> lookupAAAA(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsSrvRecord> lookupSRV(const std::string& name) const final;
		[[nodiscard]] std::vector<dnsTxtRecord> lookupTXT(const std::string& name) const final;
	};
}
