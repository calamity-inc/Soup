#include "dnsResolver.hpp"

namespace soup
{
	std::vector<IpAddr> dnsResolver::lookupIPv4(const std::string& name) const
	{
		return simplifyIPv4LookupResults(lookup(DNS_A, name));
	}

	std::vector<IpAddr> dnsResolver::lookupIPv6(const std::string& name) const
	{
		return simplifyIPv6LookupResults(lookup(DNS_AAAA, name));
	}

	std::vector<IpAddr> dnsResolver::simplifyIPv4LookupResults(const std::vector<UniquePtr<dnsRecord>>& vec)
	{
		std::vector<IpAddr> res{};
		for (const auto& r : vec)
		{
			if (r->type == DNS_A)
			{
				res.emplace_back(reinterpret_cast<dnsARecord*>(r.get())->data);
			}
		}
		return res;
	}

	std::vector<IpAddr> dnsResolver::simplifyIPv6LookupResults(const std::vector<UniquePtr<dnsRecord>>& vec)
	{
		std::vector<IpAddr> res{};
		for (const auto& r : vec)
		{
			if (r->type == DNS_AAAA)
			{
				res.emplace_back(reinterpret_cast<dnsAaaaRecord*>(r.get())->data);
			}
		}
		return res;
	}
}
