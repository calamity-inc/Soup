#include "netInfo.hpp"
#if !SOUP_WASM

#include "HttpRequest.hpp"
#include "IpAddr.hpp"
#include "netStun.hpp"

NAMESPACE_SOUP
{
	IpAddr netInfo::getPublicAddress()
	{
		return getPublicAddressImpl("api64.ipify.org");
	}

	IpAddr netInfo::getPublicAddressV4()
	{
		//return getPublicAddressImpl("whatismyip.akamai.com");
		return netStun::queryBinding(SOUP_IPV4_NWE(74, 125, 250, 129), 19302, 700);
	}

	IpAddr netInfo::getPublicAddressV6()
	{
		return getPublicAddressImpl("api6.ipify.org");
	}

	IpAddr netInfo::getPublicAddressImpl(const std::string& provider)
	{
		IpAddr addr;
		HttpRequest req(provider, "/");
		/*req.port = 80;
		req.use_tls = false;*/
		if (auto res = req.execute(); res.has_value())
		{
			addr.fromString(res->body);
		}
		return addr;
	}
}

#endif
