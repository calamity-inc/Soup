#include "netInfo.hpp"
#if !SOUP_WASM

#include "HttpRequest.hpp"
#include "IpAddr.hpp"

namespace soup
{
	IpAddr netInfo::getPublicAddress()
	{
		return getPublicAddressImpl("ip.apimon.de");
	}

	IpAddr netInfo::getPublicAddressV4()
	{
		return getPublicAddressImpl("ipv4.apimon.de");
	}

	IpAddr netInfo::getPublicAddressV6()
	{
		return getPublicAddressImpl("ipv6.apimon.de");
	}

	IpAddr netInfo::getPublicAddressImpl(const std::string& provider)
	{
		IpAddr addr;
		HttpRequest req(provider, "/");
		if (auto res = req.execute(); res.has_value())
		{
			addr.fromString(res->body);
		}
		return addr;
	}
}

#endif
