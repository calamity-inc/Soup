#include "dnsResolver.hpp"

namespace soup
{
	std::vector<IpAddr> dnsResolver::lookupIPv4(const std::string& name) const
	{
		std::vector<IpAddr> res{};
		for (const auto& r : lookup(DNS_A, name))
		{
			if (r->type == DNS_A)
			{
				res.emplace_back(reinterpret_cast<dnsARecord*>(r.get())->data);
			}
		}
		return res;
	}

	std::vector<IpAddr> dnsResolver::lookupIPv6(const std::string& name) const
	{
		std::vector<IpAddr> res{};
		for (const auto& r : lookup(DNS_AAAA, name))
		{
			if (r->type == DNS_AAAA)
			{
				res.emplace_back(reinterpret_cast<dnsAaaaRecord*>(r.get())->data);
			}
		}
		return res;
	}
}
