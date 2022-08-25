#include "dnsResolver.hpp"

namespace soup
{
	std::vector<IpAddr> dnsResolver::lookupIPv4(const std::string& name) const
	{
		std::vector<IpAddr> res{};
		for (const auto& r : lookupA(name))
		{
			res.emplace_back(r.data);
		}
		return res;
	}

	std::vector<IpAddr> dnsResolver::lookupIPv6(const std::string& name) const
	{
		std::vector<IpAddr> res{};
		for (const auto& r : lookupAAAA(name))
		{
			res.emplace_back(r.data);
		}
		return res;
	}
}
