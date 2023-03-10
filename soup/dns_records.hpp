#pragma once

#include <cstdint>
#include <string>

#include "dnsType.hpp"
#include "IpAddr.hpp"

namespace soup
{
	struct dnsRecord;

	using dnsRecordFactory = UniquePtr<dnsRecord>(*)(std::string&& name, uint32_t ttl, std::string&& human_readable_data);

	struct dnsRecord
	{
		const dnsType type;
		std::string name;
		uint32_t ttl;

		dnsRecord(dnsType type, std::string&& name, uint32_t ttl) noexcept
			: type(type), name(std::move(name)), ttl(ttl)
		{
		}

		[[nodiscard]] static dnsRecordFactory getFactory(dnsType type);

		[[nodiscard]] std::string getDataHumanReadable() const;
	};

	struct dnsARecord : public dnsRecord
	{
		network_u32_t data;

		dnsARecord(std::string&& name, uint32_t ttl, network_u32_t data) noexcept
			: dnsRecord(DNS_A, std::move(name), ttl), data(data)
		{
		}
	};

	struct dnsAaaaRecord : public dnsRecord
	{
		IpAddr data;

		dnsAaaaRecord(std::string&& name, uint32_t ttl, const uint8_t* data)
			: dnsRecord(DNS_AAAA, std::move(name), ttl), data(data)
		{
		}

		dnsAaaaRecord(std::string&& name, uint32_t ttl, const IpAddr& data)
			: dnsRecord(DNS_AAAA, std::move(name), ttl), data(data)
		{
		}
	};

	struct dnsCnameRecord : public dnsRecord
	{
		std::string data;

		dnsCnameRecord(std::string&& name, uint32_t ttl, std::string&& data)
			: dnsRecord(DNS_CNAME, std::move(name), ttl), data(std::move(data))
		{
		}
	};

	struct dnsPtrRecord : public dnsRecord
	{
		std::string data;

		dnsPtrRecord(std::string&& name, uint32_t ttl, std::string&& data)
			: dnsRecord(DNS_PTR, std::move(name), ttl), data(std::move(data))
		{
		}
	};

	struct dnsTxtRecord : public dnsRecord
	{
		std::string data;

		dnsTxtRecord(std::string&& name, uint32_t ttl, std::string&& data)
			: dnsRecord(DNS_TXT, std::move(name), ttl), data(std::move(data))
		{
		}
	};

	struct dnsMxRecord : public dnsRecord
	{
		uint16_t priority;
		std::string target;

		dnsMxRecord(std::string&& name, uint32_t ttl, uint16_t priority, std::string&& target)
			: dnsRecord(DNS_MX, std::move(name), ttl), priority(priority), target(std::move(target))
		{
		}

		[[nodiscard]] std::string getDataHumanReadable() const;
	};

	struct dnsSrvRecord : public dnsRecord
	{
		uint16_t priority;
		uint16_t weight;
		std::string target;
		uint16_t port;

		dnsSrvRecord(std::string&& name, uint32_t ttl, uint16_t priority, uint16_t weight, std::string&& target, uint16_t port)
			: dnsRecord(DNS_SRV, std::move(name), ttl), priority(priority), weight(weight), target(std::move(target)), port(port)
		{
		}

		[[nodiscard]] std::string getDataHumanReadable() const;
	};
}
