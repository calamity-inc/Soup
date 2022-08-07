#pragma once

#include <unordered_map>

#include "IpAddr.hpp"
#include "netAs.hpp"
#include "RangeMap.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	class netIntel
	{
	private:
		inline static std::unordered_map<uint32_t, UniquePtr<netAs>> aslist{};
		inline static RangeMap<uint32_t, netAs*> ipv4toas{};
		inline static RangeMap<IpAddr, netAs*> ipv6toas{};

	public:
		static void init(); // blocking
	private:
		static void initAsList();
		static void initIpv4ToAs();
		static void initIpv6ToAs();

	public:
		// Currently limited to AS with announced IPs
		[[nodiscard]] static netAs* getAsByNumber(uint32_t number) noexcept
		{
			if (auto e = aslist.find(number); e != aslist.end())
			{
				return e->second.get();
			}
			return nullptr;
		}

		[[nodiscard]] static netAs* getAsByIp(const IpAddr& addr)
		{
			return addr.isV4()
				? getAsByIpv4(addr.getV4())
				: getAsByIpv6(addr)
				;
		}

		[[nodiscard]] static netAs* getAsByIpv4(uint32_t ip)
		{
			auto e = ipv4toas.find(ip);
			if (e == nullptr)
			{
				return nullptr;
			}
			return *e;
		}

		[[nodiscard]] static netAs* getAsByIpv6(const IpAddr& addr)
		{
			auto e = ipv6toas.find(addr);
			if (e == nullptr)
			{
				return nullptr;
			}
			return *e;
		}
	};
}
