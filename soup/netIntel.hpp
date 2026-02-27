#pragma once

#include "base.hpp"
#if !SOUP_WASM || SOUP_EMSCRIPTEN

#include <string>
#include <vector>

#include "AllocRaiiFileMapping.hpp"
#include "CidrSubnetInterface.hpp"
#include "IpAddr.hpp"
#include "MemoryMappedMap.hpp"
#include "MemoryMappedRangeMap.hpp"
#include "netAs.hpp"
#include "netIntelLocationData.hpp"
#include "Optional.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	class netIntel
	{
	public:
		void init(bool ipv4 = true, bool ipv6 = true) { return initEx(ipv4, ipv6, ipv4, ipv6); } // blocking; initialises AS & location data
		void initEx(bool as_ipv4, bool as_ipv6, bool loc_ipv4, bool loc_ipv6); // blocking
		void deinit() noexcept;

		void asInit(bool ipv4 = true, bool ipv6 = true) { return initEx(ipv4, ipv6, false, false); } // blocking; initialises AS data
		[[nodiscard]] bool asIsInited() noexcept;
		void asDeinit() noexcept;
		
		void locationInit(bool ipv4 = true, bool ipv6 = true) { return initEx(false, false, ipv4, ipv6); }; // blocking; initialises location data
		[[nodiscard]] bool locationIsInited() noexcept;
		void locationDeinit() noexcept;

	protected:
		void initExtraWasm(); // blocking

	public:
		// === Lookup functions ===
		// Note that these are only available after the init has finished.
		// I point this out because of the blocking nature of the init, you would be right to put the init into another thread.
		// However, there is NO function on netIntel that tells you if the instance is currently initing; this is including the "IsInited" functions.
		// TL;DR: If there are multiple threads using the same netIntel instance, you have to control this yourself!

		[[nodiscard]] Optional<netAs> getAsByNumber(uint32_t number) const noexcept;

		[[nodiscard]] Optional<netAs> getAsByIp(const IpAddr& addr) const;
		[[nodiscard]] Optional<netAs> getAsByIpv4(native_u32_t ip) const;
		[[nodiscard]] Optional<netAs> getAsByIpv6(const IpAddr& addr) const;

		[[nodiscard]] std::vector<UniquePtr<CidrSubnetInterface>> getRangesByAsn(uint32_t asn) const;

		[[nodiscard]] Optional<netIntelLocationData> getLocationByIp(const IpAddr& addr) const;
		[[nodiscard]] Optional<netIntelLocationData> getLocationByIpv4(native_u32_t ip) const;
		[[nodiscard]] Optional<netIntelLocationData> getLocationByIpv6(const IpAddr& addr) const;

	protected:
		struct AsPoolData
		{
			uint32_t handle;
			uint32_t name;
		};
		static_assert(sizeof(MemoryMappedMap<uint32_t, AsPoolData>::Entry) == 12);

		struct LocationData
		{
			char country_code[4];
			uint32_t state;
			uint32_t city;
		};
		static_assert(sizeof(MemoryMappedRangeMap<uint32_t, LocationData>::Entry) == 20);

		MemoryMappedMap<uint32_t, AsPoolData> as_map;
		AllocRaiiFileMapping<char> as_string_pool;
		MemoryMappedRangeMap<uint32_t, uint32_t> ipv4_to_aso{};
		MemoryMappedRangeMap<IpAddr, uint32_t> ipv6_to_aso{};
	public:
		std::string extra_wasm{};
	protected:
		AllocRaiiFileMapping<char> location_string_pool;
		MemoryMappedRangeMap<uint32_t, LocationData> ipv4_to_location{};
		MemoryMappedRangeMap<IpAddr, LocationData> ipv6_to_location{};
	};
}

#endif
