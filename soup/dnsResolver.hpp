#pragma once

#include <vector>

#include "dns_records.hpp"

namespace soup
{
	struct dnsResolver
	{
		[[nodiscard]] std::vector<IpAddr> lookupIPv4(const std::string& name) const;
		[[nodiscard]] std::vector<IpAddr> lookupIPv6(const std::string& name) const;

		[[nodiscard]] virtual std::vector<dnsARecord> lookupA(const std::string& name) const = 0;
		[[nodiscard]] virtual std::vector<dnsAaaaRecord> lookupAAAA(const std::string& name) const = 0;
		[[nodiscard]] virtual std::vector<dnsSrvRecord> lookupSRV(const std::string& name) const = 0;
		[[nodiscard]] virtual std::vector<dnsTxtRecord> lookupTXT(const std::string& name) const = 0;
	};
}
