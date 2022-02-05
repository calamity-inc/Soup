#pragma once

#include <cstdint>
#include <vector>

#include "fwddecl.hpp"

namespace soup
{
	struct dns
	{
		// simple lookups

		[[nodiscard]] static std::vector<addr_ip> lookupIPv4(const char* name) noexcept;
		[[nodiscard]] static std::vector<addr_ip> lookupIPv6(const char* name) noexcept;

		// direct record lookups

		[[nodiscard]] static std::vector<dns_a> lookupA(const char* name) noexcept;
		[[nodiscard]] static std::vector<dns_aaaa> lookupAAAA(const char* name) noexcept;
		[[nodiscard]] static std::vector<dns_srv> lookupSRV(const char* name) noexcept;
		[[nodiscard]] static std::vector<dns_txt> lookupTXT(const char* name) noexcept;

		// results struct

		uint32_t ttl;

		constexpr dns(uint32_t ttl) noexcept
			: ttl(ttl)
		{
		}
	};
}
