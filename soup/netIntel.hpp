#pragma once

#include <filesystem>
#include <unordered_map>

#include "AllocRaiiFileMapping.hpp"
#include "CidrSubnetInterface.hpp"
#include "IpAddr.hpp"
#include "MemoryMappedMap.hpp"
#include "MemoryMappedRangeMap.hpp"
#include "netAs.hpp"
#include "netIntelLocationData.hpp"
#include "Optional.hpp"
#include "RangeMap.hpp"
#include "StringPool.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	class netIntel
	{
	public:
		void init(bool ipv4 = true, bool ipv6 = true); // blocking; initialises AS & location data
		void deinit() noexcept;

		void asInit(bool ipv4 = true, bool ipv6 = true); // blocking; initialises AS data
		[[nodiscard]] bool asIsInited() noexcept;
		void asDeinit() noexcept;
		
		void locationInit(bool ipv4 = true, bool ipv6 = true); // blocking; initialises location data
		[[nodiscard]] bool locationIsInited() noexcept;
		void locationDeinit() noexcept;

	private:
		void initExtraWasm(); // blocking
		void initIpv4ToLocation(); // blocking
		void initIpv6ToLocation(); // blocking

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

		[[nodiscard]] const netIntelLocationData* getLocationByIp(const IpAddr& addr) const;
		[[nodiscard]] const netIntelLocationData* getLocationByIpv4(native_u32_t ip) const;
		[[nodiscard]] const netIntelLocationData* getLocationByIpv6(const IpAddr& addr) const;

	public:
		void locationExport(const std::filesystem::path& dir);

	protected:
		struct AsPoolData
		{
			uint32_t handle;
			uint32_t name;
		};
		static_assert(sizeof(MemoryMappedMap<uint32_t, AsPoolData>::Entry) == 12);

		MemoryMappedMap<uint32_t, AsPoolData> as_map;
		AllocRaiiFileMapping<char> as_string_pool;
		MemoryMappedRangeMap<uint32_t, uint32_t> ipv4_to_aso{};
		MemoryMappedRangeMap<IpAddr, uint32_t> ipv6_to_aso{};
	public:
		std::string extra_wasm{};
		StringPool location_pool{};
		RangeMap<uint32_t, netIntelLocationData> ipv4tolocation{};
		RangeMap<IpAddr, netIntelLocationData> ipv6tolocation{};
	};
}
