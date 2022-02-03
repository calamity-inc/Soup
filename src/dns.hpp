#pragma once

#include <vector>

#include "addr_ip.hpp"
#include "platform.hpp"

#if SOUP_PLATFORM_WINDOWS
#include "WinDNS.h"
#pragma comment(lib, "Dnsapi.lib")
#endif

namespace soup
{
	struct dns
	{
		
		[[nodiscard]] static std::vector<addr_ip> lookup(const char* name) noexcept
		{
			std::vector<addr_ip> res = lookupIPv6(name);
			std::vector<addr_ip> res_v4 = lookupIPv4(name);
			res.insert(res.end(), std::make_move_iterator(res_v4.begin()), std::make_move_iterator(res_v4.end()));
			return res;
		}

		[[nodiscard]] static std::vector<addr_ip> lookupIPv6(const char* name) noexcept
		{
			std::vector<addr_ip> res{};
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

		[[nodiscard]] static std::vector<addr_ip> lookupIPv4(const char* name) noexcept
		{
			std::vector<addr_ip> res{};
#if SOUP_PLATFORM_WINDOWS
			PDNS_RECORD pDnsRecord;
			if (DnsQuery_UTF8(name, DNS_TYPE_A, DNS_QUERY_STANDARD, 0, &pDnsRecord, 0) == ERROR_SUCCESS)
			{
				for (PDNS_RECORD i = pDnsRecord; i; i = i->pNext)
				{
					res.emplace_back(i->Data.A.IpAddress);
				}
			}
#else
			// TODO https://linux.die.net/man/3/res_query
#endif
			return res;
		}
	};
}
