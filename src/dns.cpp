#include "dns.hpp"

#include "dns_a.hpp"
#include "dns_aaaa.hpp"
#include "dns_srv.hpp"
#include "dns_txt.hpp"

#include "platform.hpp"
#if SOUP_PLATFORM_WINDOWS
#include "WinDNS.h"
#pragma comment(lib, "Dnsapi.lib")
#else
#include <resolv.h>
#endif

namespace soup
{
	std::vector<addr_ip> dns::lookupIPv4(const char* name) noexcept
	{
		std::vector<addr_ip> res{};
		for (const auto& r : lookupA(name))
		{
			res.emplace_back(r.data);
		}
		return res;
	}

	std::vector<addr_ip> dns::lookupIPv6(const char* name) noexcept
	{
		std::vector<addr_ip> res{};
		for (const auto& r : lookupAAAA(name))
		{
			res.emplace_back(r.data);
		}
		return res;
	}

	std::vector<dns_a> dns::lookupA(const char* name) noexcept
	{
		std::vector<dns_a> res{};
#if SOUP_PLATFORM_WINDOWS
		PDNS_RECORD pDnsRecord;
		if (DnsQuery_UTF8(name, DNS_TYPE_A, DNS_QUERY_STANDARD, 0, &pDnsRecord, 0) == ERROR_SUCCESS)
		{
			for (PDNS_RECORD i = pDnsRecord; i; i = i->pNext)
			{
				res.emplace_back(i->dwTtl, i->Data.A.IpAddress);
			}
		}
#else
		unsigned char query_buffer[1024];
		auto ret = res_query(name, C_IN, ns_t_a, query_buffer, sizeof(query_buffer));
		if (ret > 0)
		{
			ns_msg nsMsg;
			ns_initparse(query_buffer, ret, &nsMsg);
			for (int i = 0; i < ns_msg_count(nsMsg, ns_s_an); ++i)
			{
				ns_rr rr;
				ns_parserr(&nsMsg, ns_s_an, i, &rr);
				res.emplace_back(ns_rr_ttl(rr), *(const uint32_t*)ns_rr_rdata(rr));
			}
		}
#endif
		return res;
	}

	std::vector<dns_aaaa> dns::lookupAAAA(const char* name) noexcept
	{
		std::vector<dns_aaaa> res{};
#if SOUP_PLATFORM_WINDOWS
		PDNS_RECORD pDnsRecord;
		if (DnsQuery_UTF8(name, DNS_TYPE_AAAA, DNS_QUERY_STANDARD, 0, &pDnsRecord, 0) == ERROR_SUCCESS)
		{
			for (PDNS_RECORD i = pDnsRecord; i; i = i->pNext)
			{
				res.emplace_back(i->dwTtl, i->Data.AAAA.Ip6Address.IP6Byte);
			}
		}
#else
		unsigned char query_buffer[1024];
		auto ret = res_query(name, C_IN, ns_t_aaaa, query_buffer, sizeof(query_buffer));
		if (ret > 0)
		{
			ns_msg nsMsg;
			ns_initparse(query_buffer, ret, &nsMsg);
			for (int i = 0; i < ns_msg_count(nsMsg, ns_s_an); ++i)
			{
				ns_rr rr;
				ns_parserr(&nsMsg, ns_s_an, i, &rr);
				res.emplace_back(ns_rr_ttl(rr), (const uint8_t*)ns_rr_rdata(rr));
			}
		}
#endif
		return res;
	}

	std::vector<dns_srv> dns::lookupSRV(const char* name) noexcept
	{
		std::vector<dns_srv> res{};
#if SOUP_PLATFORM_WINDOWS
		PDNS_RECORD pDnsRecord;
		if (DnsQuery_UTF8(name, DNS_TYPE_SRV, DNS_QUERY_STANDARD, 0, &pDnsRecord, 0) == ERROR_SUCCESS)
		{
			for (PDNS_RECORD i = pDnsRecord; i; i = i->pNext)
			{
				res.emplace_back(i->dwTtl, i->Data.SRV.wPriority, i->Data.SRV.wWeight, i->Data.SRV.pNameTarget, i->Data.SRV.wPort);
			}
		}
#else
		unsigned char query_buffer[1024];
		auto ret = res_query(name, C_IN, ns_t_srv, query_buffer, sizeof(query_buffer));
		if (ret > 0)
		{
			ns_msg nsMsg;
			ns_initparse(query_buffer, ret, &nsMsg);
			for (int i = 0; i < ns_msg_count(nsMsg, ns_s_an); ++i)
			{
				ns_rr rr;
				ns_parserr(&nsMsg, ns_s_an, i, &rr);
				char target[1024];
				dn_expand(ns_msg_base(nsMsg), ns_msg_end(nsMsg), ns_rr_rdata(rr) + 6, target, sizeof(target));
				res.emplace_back(ns_rr_ttl(rr), ntohs(*(unsigned short*)ns_rr_rdata(rr)), ntohs(*((unsigned short*)ns_rr_rdata(rr) + 1)), target, ntohs(*((unsigned short*)ns_rr_rdata(rr) + 2)));
			}
		}
#endif
		return res;
	}

	std::vector<dns_txt> dns::lookupTXT(const char* name) noexcept
	{
		std::vector<dns_txt> res{};
#if SOUP_PLATFORM_WINDOWS
		PDNS_RECORD pDnsRecord;
		if (DnsQuery_UTF8(name, DNS_TYPE_TEXT, DNS_QUERY_STANDARD, 0, &pDnsRecord, 0) == ERROR_SUCCESS)
		{
			for (PDNS_RECORD i = pDnsRecord; i; i = i->pNext)
			{
				res.emplace_back(i->dwTtl, i->Data.TXT.pStringArray[0]);
			}
		}
#else
		unsigned char query_buffer[1024];
		auto ret = res_query(name, C_IN, ns_t_txt, query_buffer, sizeof(query_buffer));
		if (ret > 0)
		{
			ns_msg nsMsg;
			ns_initparse(query_buffer, ret, &nsMsg);
			for (int i = 0; i < ns_msg_count(nsMsg, ns_s_an); ++i)
			{
				ns_rr rr;
				ns_parserr(&nsMsg, ns_s_an, i, &rr);
				res.emplace_back(ns_rr_ttl(rr), (const char*)(ns_rr_rdata(rr) + 1));
			}
		}
#endif
		return res;
	}
}
