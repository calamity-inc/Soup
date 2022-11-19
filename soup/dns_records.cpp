#include "dns_records.hpp"

#include "UniquePtr.hpp"

namespace soup
{
	dnsRecordFactory dnsRecord::getFactory(dnsType type)
	{
		switch (type)
		{
		case DNS_A:
			return [](std::string&& name, uint32_t ttl, std::string&& human_readable_data) -> UniquePtr<dnsRecord>
			{
				return soup::make_unique<dnsARecord>(std::move(name), ttl, IpAddr(human_readable_data).getV4());
			};

		case DNS_AAAA:
			return [](std::string&& name, uint32_t ttl, std::string&& human_readable_data) -> UniquePtr<dnsRecord>
			{
				return soup::make_unique<dnsAaaaRecord>(std::move(name), ttl, IpAddr(human_readable_data));
			};

		default:;
		}
		return nullptr;
	}

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
