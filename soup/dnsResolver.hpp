#pragma once

#include <vector>

#include "dnsLookupTask.hpp"
#include "dns_records.hpp"
#include "UniquePtr.hpp"

#ifndef NDEBUG
#include "TransientToken.hpp"
#endif

namespace soup
{
	struct dnsResolver
	{
#ifndef NDEBUG
		TransientToken transient_token;
#endif

		virtual ~dnsResolver() = default;

		[[nodiscard]] std::vector<IpAddr> lookupIPv4(const std::string& name) const;
		[[nodiscard]] std::vector<IpAddr> lookupIPv6(const std::string& name) const;

		[[nodiscard]] virtual std::vector<UniquePtr<dnsRecord>> lookup(dnsType qtype, const std::string& name) const = 0;
		[[nodiscard]] virtual UniquePtr<dnsLookupTask> makeLookupTask(dnsType qtype, const std::string& name) const;

		[[nodiscard]] static std::vector<IpAddr> simplifyIPv4LookupResults(const std::vector<UniquePtr<dnsRecord>>& vec);
		[[nodiscard]] static std::vector<IpAddr> simplifyIPv6LookupResults(const std::vector<UniquePtr<dnsRecord>>& vec);
	};
}
