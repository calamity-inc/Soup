#include "netAdaptor.hpp"

#if SOUP_WINDOWS || SOUP_LINUX

#if SOUP_WINDOWS
	#include <windows.h>
	#include <iphlpapi.h>
	#pragma comment(lib, "iphlpapi.lib")

	#include "AllocRaii.hpp"
#else
	#include <ifaddrs.h> // getifaddrs
	#include <linux/if_packet.h> // sockaddr_ll
#endif
#include "IpAddr.hpp"
#include "MacAddr.hpp"

NAMESPACE_SOUP
{
	std::vector<netAdaptor> netAdaptor::getAll()
	{
		std::vector<netAdaptor> res;
	#if SOUP_WINDOWS
		ULONG buf_len = 0;
		SOUP_ASSERT(GetAdaptersInfo(nullptr, &buf_len) == ERROR_BUFFER_OVERFLOW);
		AllocRaii buf(buf_len);
		PIP_ADAPTER_INFO adaptor = (IP_ADAPTER_INFO*)buf.operator void*();
		SOUP_ASSERT(GetAdaptersInfo(adaptor, &buf_len) == NO_ERROR);
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
	#else
		struct ifaddrs* ifaddr;
		SOUP_RETHROW_FALSE(getifaddrs(&ifaddr) != -1);
		for (struct ifaddrs* ifa = ifaddr; ifa; ifa = ifa->ifa_next)
		{
			if (
				ifa->ifa_addr
				&& ifa->ifa_addr->sa_family == AF_INET
				)
			{
				netAdaptor& na = res.emplace_back(netAdaptor{});
				na.name = ifa->ifa_name;

				struct sockaddr_in* sa = (struct sockaddr_in*)ifa->ifa_addr;
				na.ip_addr = Endianness::toNative((network_u32_t)sa->sin_addr.s_addr);

				struct sockaddr_in* nm = (struct sockaddr_in*)ifa->ifa_netmask;
				na.netmask = Endianness::toNative((network_u32_t)nm->sin_addr.s_addr);

				struct ifaddrs* mac_ifa;
				for (mac_ifa = ifaddr; mac_ifa; mac_ifa = mac_ifa->ifa_next)
				{
					if (
						mac_ifa->ifa_addr
						&& strcmp(mac_ifa->ifa_name, na.name.c_str()) == 0
						&& mac_ifa->ifa_addr->sa_family == AF_PACKET
						)
					{
						struct sockaddr_ll* s = (struct sockaddr_ll*)mac_ifa->ifa_addr;
						if (s->sll_halen == 6)
						{
							na.mac_addr = MacAddr(s->sll_addr);
						}
						break;
					}
				}
			}
		}
		freeifaddrs(ifaddr);
	#endif
		return res;
	}

	bool netAdaptor::isVirtual() const noexcept
	{
#if SOUP_WINDOWS
		return name.find("Virtual") != std::string::npos;
#else
		return name == "lo";
#endif
	}
}

#endif
