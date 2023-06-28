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
		StringPool as_pool{};
		StringPool location_pool{};
		std::unordered_map<uint32_t, UniquePtr<netAs>> aslist{};
		RangeMap<uint32_t, const netAs*> ipv4toas{};
		RangeMap<IpAddr, const netAs*> ipv6toas{};
		RangeMap<uint32_t, netIntelLocationData> ipv4tolocation{};
		RangeMap<IpAddr, netIntelLocationData> ipv6tolocation{};

	public:
		void init(bool ipv4 = true, bool ipv6 = true); // blocking; initialises AS & location data
		
		void asInit(bool ipv4 = true, bool ipv6 = true); // blocking; initialises AS data
		[[nodiscard]] bool asIsInited() noexcept;
		void asDeinit() noexcept;
		
		void locationInit(bool ipv4 = true, bool ipv6 = true); // blocking; initialises location data
		[[nodiscard]] bool locationIsInited() noexcept;
		void locationDeinit() noexcept;

	private:
		void initAsList(); // blocking
		void initIpv4ToAs(); // blocking
		void initIpv6ToAs(); // blocking
		void initIpv4ToLocation(); // blocking
		void initIpv6ToLocation(); // blocking

	public:
		[[nodiscard]] const netAs* getAsByNumber(uint32_t number) const noexcept;

		[[nodiscard]] const netAs* getAsByIp(const IpAddr& addr) const;
		[[nodiscard]] const netAs* getAsByIpv4(native_u32_t ip) const;
		[[nodiscard]] const netAs* getAsByIpv6(const IpAddr& addr) const;

		[[nodiscard]] const netIntelLocationData* getLocationByIp(const IpAddr& addr) const;
		[[nodiscard]] const netIntelLocationData* getLocationByIpv4(native_u32_t ip) const;
		[[nodiscard]] const netIntelLocationData* getLocationByIpv6(const IpAddr& addr) const;
	};
}
