#include "netAdaptor.hpp"

#if SOUP_WINDOWS
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include "AllocRaii.hpp"
#include "IpAddr.hpp"
#include "MacAddr.hpp"

NAMESPACE_SOUP
{
	std::vector<netAdaptor> netAdaptor::getAll()
	{
		ULONG buf_len = 0;
		SOUP_ASSERT(GetAdaptersInfo(nullptr, &buf_len) == ERROR_BUFFER_OVERFLOW);
		AllocRaii buf(buf_len);
		PIP_ADAPTER_INFO adaptor = (IP_ADAPTER_INFO*)buf.operator void*();
		SOUP_ASSERT(GetAdaptersInfo(adaptor, &buf_len) == NO_ERROR);
		std::vector<netAdaptor> res;
		for (; adaptor; adaptor = adaptor->Next)
		{
			if (adaptor->Type == MIB_IF_TYPE_ETHERNET
				&& adaptor->AddressLength == 6 // Should always hold true for ethernet, but just to be sure...
				)
			{
				netAdaptor& na = res.emplace_back(netAdaptor{});
				na.name = adaptor->Description; // "AdapterName" is just some GUID, "Description" is the actual name
				na.mac_addr = MacAddr(adaptor->Address);
				if (IpAddr addr; addr.fromString(adaptor->IpAddressList.IpAddress.String))
				{
					na.ip_addr = addr.getV4NativeEndian();
				}
				if (IpAddr addr; addr.fromString(adaptor->IpAddressList.IpMask.String))
				{
					na.netmask = addr.getV4NativeEndian();
				}
			}
		}
		return res;
	}
}

#endif
