#pragma once

#include <vector>

#include "net_addr_ip.hpp"
#include "platform.hpp"

#if SOUP_PLATFORM_WINDOWS
#include "WinDNS.h"
#pragma comment(lib, "Dnsapi.lib")
#endif

namespace soup
{
	struct dns
	{
		[[nodiscard]] static std::vector<net_addr_ip> lookupAAAA(const char* name)
		{
			std::vector<net_addr_ip> res{};
#if SOUP_PLATFORM_WINDOWS
			PDNS_RECORD pDnsRecord;
			if (DnsQuery_UTF8(name, DNS_TYPE_AAAA, DNS_QUERY_STANDARD, 0, &pDnsRecord, 0) == ERROR_SUCCESS)
			{
				for (PDNS_RECORD i = pDnsRecord; i; i = i->pNext)
				{
					res.emplace_back(i->Data.AAAA.Ip6Address.IP6Byte);
				}
			}
#else
			// TODO https://linux.die.net/man/3/res_query
#endif
			return res;
		}
	};
}
