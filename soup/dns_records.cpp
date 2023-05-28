#include "dns_records.hpp"

#include "dnsName.hpp"
#include "Exception.hpp"
#include "string.hpp"
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

		case DNS_CNAME:
			return [](std::string&& name, uint32_t ttl, std::string&& human_readable_data) -> UniquePtr<dnsRecord>
			{
				return soup::make_unique<dnsCnameRecord>(std::move(name), ttl, std::move(human_readable_data));
			};

		case DNS_PTR:
			return [](std::string&& name, uint32_t ttl, std::string&& human_readable_data) -> UniquePtr<dnsRecord>
			{
				return soup::make_unique<dnsPtrRecord>(std::move(name), ttl, std::move(human_readable_data));
			};

		case DNS_TXT:
			return [](std::string&& name, uint32_t ttl, std::string&& human_readable_data) -> UniquePtr<dnsRecord>
			{
				return soup::make_unique<dnsTxtRecord>(std::move(name), ttl, std::move(human_readable_data));
			};

		case DNS_NS:
			return [](std::string&& name, uint32_t ttl, std::string&& human_readable_data) -> UniquePtr<dnsRecord>
			{
				return soup::make_unique<dnsNsRecord>(std::move(name), ttl, std::move(human_readable_data));
			};

		default:;
		}
		return nullptr;
	}

	std::string dnsRecordName::toRdata() const
	{
		dnsName value;
		value.name = string::explode(data, '.');

		StringWriter sw(false);
		value.write(sw);
		return sw.data;
	}

	std::string dnsMxRecord::getDataHumanReadable() const
	{
		std::string str = std::to_string(priority);
		str.push_back(' ');
		str.append(target);
		return str;
	}

	std::string dnsMxRecord::toRdata() const
	{
		Exception::purecall();
	}

	std::string dnsSrvRecord::getDataHumanReadable() const
	{
		std::string str = std::to_string(priority);
		str.push_back(' ');
		str.append(std::to_string(weight));
		str.push_back(' ');
		str.append(std::to_string(port));
		str.push_back(' ');
		str.append(target);
		return str;
	}

	std::string dnsSrvRecord::toRdata() const
	{
		Exception::purecall();
	}
}
