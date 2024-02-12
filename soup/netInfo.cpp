#include "netInfo.hpp"
#if !SOUP_WASM

#include "HttpRequest.hpp"
#include "IpAddr.hpp"

namespace soup
{
	IpAddr netInfo::getPublicAddress()
	{
		return getPublicAddressImpl("myi.pe");
	}

	IpAddr netInfo::getPublicAddressV4()
	{
		return getPublicAddressImpl("v4.myi.pe");
	}

	IpAddr netInfo::getPublicAddressV6()
	{
		return getPublicAddressImpl("v6.myi.pe");
	}

	IpAddr netInfo::getPublicAddressImpl(const std::string& provider)
	{
		IpAddr addr;
		HttpRequest req(provider, "/get");
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
