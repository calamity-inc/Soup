#pragma once

#include "IpAddr.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace soup::dns
{
	struct Record
	{
		uint32_t ttl;

		constexpr Record(uint32_t ttl) noexcept
			: ttl(ttl)
		{
		}
	};

	struct ARecord : public Record
	{
		uint32_t data;

		constexpr ARecord(uint32_t ttl, uint32_t data)
			: Record(ttl), data(data)
		{
		}
	};

	struct AaaaRecord : public Record
	{
		IpAddr data;

		AaaaRecord(uint32_t ttl, const uint8_t* data)
			: Record(ttl), data(data)
		{
		}
	};

	struct SrvRecord : public Record
	{
		uint16_t priority;
		uint16_t weight;
		std::string target;
		uint16_t port;

		SrvRecord(uint32_t ttl, uint16_t priority, uint16_t weight, std::string&& target, uint16_t port)
			: Record(ttl), priority(priority), weight(weight), target(std::move(target)), port(port)
		{
		}
	};

	struct TxtRecord : public Record
	{
		std::string data;

		TxtRecord(uint32_t ttl, std::string&& data)
			: Record(ttl), data(std::move(data))
		{
		}
	};

	[[nodiscard]] std::vector<IpAddr> lookupIPv4(const char* name) noexcept;
	[[nodiscard]] std::vector<IpAddr> lookupIPv6(const char* name) noexcept;

	[[nodiscard]] std::vector<ARecord> lookupA(const char* name) noexcept;
	[[nodiscard]] std::vector<AaaaRecord> lookupAAAA(const char* name) noexcept;
	[[nodiscard]] std::vector<SrvRecord> lookupSRV(const char* name) noexcept;
	[[nodiscard]] std::vector<TxtRecord> lookupTXT(const char* name) noexcept;
}
