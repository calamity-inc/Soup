#include "dnsOsResolver.hpp"

#include "base.hpp"
#if SOUP_WINDOWS
#include "WinDNS.h"
#pragma comment(lib, "Dnsapi.lib")
#else
#include <resolv.h>
#endif

namespace soup
{
	std::vector<UniquePtr<dnsRecord>> dnsOsResolver::lookup(dnsType qtype, const std::string& name) const
	{
		std::vector<UniquePtr<dnsRecord>> res{};
#if SOUP_WINDOWS
		PDNS_RECORD pDnsRecord;
		if (DnsQuery_UTF8(name.c_str(), qtype, DNS_QUERY_STANDARD, 0, &pDnsRecord, 0) == ERROR_SUCCESS)
		{
			for (PDNS_RECORD i = pDnsRecord; i; i = i->pNext)
			{
				if (i->wType == DNS_TYPE_A)
				{
					res.emplace_back(soup::make_unique<dnsARecord>(i->pName, i->dwTtl, i->Data.A.IpAddress));
				}
				else if (i->wType == DNS_TYPE_AAAA)
				{
					res.emplace_back(soup::make_unique<dnsAaaaRecord>(i->pName, i->dwTtl, i->Data.AAAA.Ip6Address.IP6Byte));
				}
				else if (i->wType == DNS_TYPE_CNAME)
				{
					res.emplace_back(soup::make_unique<dnsCnameRecord>(i->pName, i->dwTtl, i->Data.CNAME.pNameHost));
				}
				else if (i->wType == DNS_TYPE_TEXT)
				{
					res.emplace_back(soup::make_unique<dnsTxtRecord>(i->pName, i->dwTtl, i->Data.TXT.pStringArray[0]));
				}
				else if (i->wType == DNS_TYPE_SRV)
				{
					res.emplace_back(soup::make_unique<dnsSrvRecord>(i->pName, i->dwTtl, i->Data.SRV.wPriority, i->Data.SRV.wWeight, i->Data.SRV.pNameTarget, i->Data.SRV.wPort));
				}
			}
			DnsRecordListFree(pDnsRecord, DnsFreeRecordListDeep);
		}
#else
		unsigned char query_buffer[1024];
		auto ret = res_query(name.c_str(), C_IN, qtype, query_buffer, sizeof(query_buffer));
		if (ret > 0)
		{
			ns_msg nsMsg;
			ns_initparse(query_buffer, ret, &nsMsg);
			for (int i = 0; i < ns_msg_count(nsMsg, ns_s_an); ++i)
			{
				ns_rr rr;
				ns_parserr(&nsMsg, ns_s_an, i, &rr);
				if (ns_rr_type(rr) == ns_t_a)
				{
					res.emplace_back(soup::make_unique<dnsARecord>(ns_rr_name(rr), ns_rr_ttl(rr), *(const uint32_t*)ns_rr_rdata(rr)));
				}
				else if (ns_rr_type(rr) == ns_t_aaaa)
				{
					res.emplace_back(soup::make_unique<dnsAaaaRecord>(ns_rr_name(rr), ns_rr_ttl(rr), (const uint8_t*)ns_rr_rdata(rr)));
				}
				else if (ns_rr_type(rr) == ns_t_cname)
				{
					char cname[1024];
					dn_expand(ns_msg_base(nsMsg), ns_msg_end(nsMsg), ns_rr_rdata(rr), cname, sizeof(cname));
					res.emplace_back(soup::make_unique<dnsCnameRecord>(ns_rr_name(rr), ns_rr_ttl(rr), cname));
				}
				else if (ns_rr_type(rr) == ns_t_txt)
				{
					res.emplace_back(soup::make_unique<dnsTxtRecord>(ns_rr_name(rr), ns_rr_ttl(rr), (const char*)(ns_rr_rdata(rr) + 1)));
				}
				else if (ns_rr_type(rr) == ns_t_srv)
				{
					char target[1024];
					dn_expand(ns_msg_base(nsMsg), ns_msg_end(nsMsg), ns_rr_rdata(rr) + 6, target, sizeof(target));
					res.emplace_back(soup::make_unique<dnsSrvRecord>(ns_rr_name(rr), ns_rr_ttl(rr), ntohs(*(unsigned short*)ns_rr_rdata(rr)), ntohs(*((unsigned short*)ns_rr_rdata(rr) + 1)), target, ntohs(*((unsigned short*)ns_rr_rdata(rr) + 2))));
				}
			}
		}
#endif
		return res;
	}
}
