#pragma once

#include <unordered_map>

#include "IpAddr.hpp"
#include "netAs.hpp"
#include "netIntelLocationData.hpp"
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
		inline static RangeMap<uint32_t, netIntelLocationData> ipv4tolocation{};
		inline static RangeMap<IpAddr, netIntelLocationData> ipv6tolocation{};

	public:
		static void init(); // blocking; initialises AS & location data
		static void initAs(); // blocking; initialises AS data
		static void initLocation(); // blocking; initialises location data
	private:
		static void initAsList(); // blocking
		static void initIpv4ToAs(); // blocking
		static void initIpv6ToAs(); // blocking
		static void initIpv4ToLocation(); // blocking
		static void initIpv6ToLocation(); // blocking

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
				? getAsByIpv4(addr.getV4NativeEndian())
				: getAsByIpv6(addr)
				;
		}

		[[nodiscard]] static netIntelLocationData* getLocationByIp(const IpAddr& addr)
		{
			return addr.isV4()
				? ipv4tolocation.find(addr.getV4NativeEndian())
				: ipv6tolocation.find(addr)
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
