#include "netInfo.hpp"
#if !SOUP_WASM

#include "HttpRequest.hpp"
#include "IpAddr.hpp"

namespace soup
{
	IpAddr netInfo::getPublicAddress()
	{
		return getPublicAddressImpl("api64.ipify.org");
	}

	IpAddr netInfo::getPublicAddressV4()
	{
		return getPublicAddressImpl("api.ipify.org");
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
