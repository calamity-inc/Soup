#include "dns_records.hpp"

namespace soup
{
	std::string dnsRecord::getValueString() const
	{
		switch (type)
		{
		case DNS_A: return IpAddr(reinterpret_cast<const dnsARecord*>(this)->data).toString4();
		case DNS_AAAA: return reinterpret_cast<const dnsAaaaRecord*>(this)->data.toString6();
		case DNS_CNAME: return reinterpret_cast<const dnsCnameRecord*>(this)->data;
		case DNS_PTR: return reinterpret_cast<const dnsPtrRecord*>(this)->data;
		case DNS_TXT: return reinterpret_cast<const dnsTxtRecord*>(this)->data;
		default:;
		}
		return {};
	}
}
