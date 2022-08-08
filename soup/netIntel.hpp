#pragma once

#include <unordered_map>

#include "IpAddr.hpp"
#include "netAs.hpp"
#include "netIntelLocationData.hpp"
#include "RangeMap.hpp"
#include "StringPool.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	class netIntel
	{
	private:
		inline static StringPool as_pool{};
		inline static StringPool location_pool{};
		inline static std::unordered_map<uint32_t, UniquePtr<netAs>> aslist{};
		inline static RangeMap<uint32_t, netAs*> ipv4toas{};
		inline static RangeMap<IpAddr, netAs*> ipv6toas{};
		inline static RangeMap<uint32_t, netIntelLocationData> ipv4tolocation{};
		inline static RangeMap<IpAddr, netIntelLocationData> ipv6tolocation{};

	public:
		static void init(bool ipv4 = true, bool ipv6 = true); // blocking; initialises AS & location data
		static void initAs(bool ipv4 = true, bool ipv6 = true); // blocking; initialises AS data
		static void initLocation(bool ipv4 = true, bool ipv6 = true); // blocking; initialises location data
	private:
		static void initAsList(); // blocking
		static void initIpv4ToAs(); // blocking
		static void initIpv6ToAs(); // blocking
		static void initIpv4ToLocation(); // blocking
		static void initIpv6ToLocation(); // blocking

	public:
		[[nodiscard]] static netAs* getAsByNumber(uint32_t number) noexcept;

		[[nodiscard]] static netAs* getAsByIp(const IpAddr& addr);
		[[nodiscard]] static netAs* getAsByIpv4(uint32_t ip);
		[[nodiscard]] static netAs* getAsByIpv6(const IpAddr& addr);

		[[nodiscard]] static netIntelLocationData* getLocationByIp(const IpAddr& addr);
		[[nodiscard]] static netIntelLocationData* getLocationByIpv4(uint32_t ip);
		[[nodiscard]] static netIntelLocationData* getLocationByIpv6(const IpAddr& addr);
	};
}
