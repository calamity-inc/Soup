#pragma once

#include "VirtualDtorBase.hpp"

#include <vector>

#include "dns_records.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct dnsResolver : public VirtualDtorBase
	{
		[[nodiscard]] std::vector<IpAddr> lookupIPv4(const std::string& name) const;
		[[nodiscard]] std::vector<IpAddr> lookupIPv6(const std::string& name) const;

		[[nodiscard]] virtual std::vector<UniquePtr<dnsRecord>> lookup(dnsType qtype, const std::string& name) const = 0;
	};
}
