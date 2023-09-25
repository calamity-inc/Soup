#pragma once

#include <filesystem>
#include <unordered_map>

#include "CidrSubnetInterface.hpp"
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
	public:
		StringPool as_pool{};
		StringPool location_pool{};
		std::unordered_map<uint32_t, UniquePtr<netAs>> aslist{};
		RangeMap<uint32_t, const netAs*> ipv4toas{};
		RangeMap<IpAddr, const netAs*> ipv6toas{};
		RangeMap<uint32_t, netIntelLocationData> ipv4tolocation{};
		RangeMap<IpAddr, netIntelLocationData> ipv6tolocation{};

		void init(bool ipv4 = true, bool ipv6 = true); // blocking; initialises AS & location data
		void deinit() noexcept;

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
		// === Lookup functions ===
		// Note that these are only available after the init has finished.
		// I point this out because of the blocking nature of the init, you would be right to put the init into another thread.
		// However, there is NO function on netIntel that tells you if the instance is currently initing; this is including the "IsInited" functions.
		// TL;DR: If there are multiple threads using the same netIntel instance, you have to control this yourself!

		[[nodiscard]] const netAs* getAsByNumber(uint32_t number) const noexcept;

		[[nodiscard]] const netAs* getAsByIp(const IpAddr& addr) const;
		[[nodiscard]] const netAs* getAsByIpv4(native_u32_t ip) const;
		[[nodiscard]] const netAs* getAsByIpv6(const IpAddr& addr) const;

		[[nodiscard]] std::vector<UniquePtr<CidrSubnetInterface>> getRangesByAs(const netAs* as) const;

		[[nodiscard]] const netIntelLocationData* getLocationByIp(const IpAddr& addr) const;
		[[nodiscard]] const netIntelLocationData* getLocationByIpv4(native_u32_t ip) const;
		[[nodiscard]] const netIntelLocationData* getLocationByIpv6(const IpAddr& addr) const;

	public:
		void locationExport(const std::filesystem::path& dir);
	};
}
