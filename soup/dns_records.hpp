#pragma once

#include <cstdint>
#include <string>

#include "IpAddr.hpp"

namespace soup
{
	struct dnsRecord
	{
		uint32_t ttl;

		constexpr dnsRecord(uint32_t ttl) noexcept
			: ttl(ttl)
		{
		}
	};

	struct dnsARecord : public dnsRecord
	{
		uint32_t data;

		constexpr dnsARecord(uint32_t ttl, uint32_t data)
			: dnsRecord(ttl), data(data)
		{
		}
	};

	struct dnsAaaaRecord : public dnsRecord
	{
		IpAddr data;

		dnsAaaaRecord(uint32_t ttl, const uint8_t* data)
			: dnsRecord(ttl), data(data)
		{
		}
	};

	struct dnsSrvRecord : public dnsRecord
	{
		uint16_t priority;
		uint16_t weight;
		std::string target;
		uint16_t port;

		dnsSrvRecord(uint32_t ttl, uint16_t priority, uint16_t weight, std::string&& target, uint16_t port)
			: dnsRecord(ttl), priority(priority), weight(weight), target(std::move(target)), port(port)
		{
		}
	};

	struct dnsTxtRecord : public dnsRecord
	{
		std::string data;

		dnsTxtRecord(uint32_t ttl, std::string&& data)
			: dnsRecord(ttl), data(std::move(data))
		{
		}
	};
}
