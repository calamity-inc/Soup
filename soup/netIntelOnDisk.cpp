#include "netIntelOnDisk.hpp"

#include "netIntelLocationData4OnDisk.hpp"

NAMESPACE_SOUP
{
	netIntelOnDisk::netIntelOnDisk(const std::filesystem::path& dir)
		: location_pool(dir / "location_pool.bin"),
		ipv4tolocation_fr(dir / "ipv4tolocation.bin"),
		ipv4tolocation_dr(ipv4tolocation_fr, measurePacket<netIntelLocationData4OnDisk>())
	{
	}

	netIntelLocationDataSelfContained netIntelOnDisk::getLocationByIpv4(native_u32_t ip)
	{
		netIntelLocationDataSelfContained res;

		if (ipv4tolocation_dr.seekEntry<uint32_t>(ip, &netIntelLocationData4OnDisk::cmp))
		{
			netIntelLocationData4OnDisk data;
			data.read(ipv4tolocation_fr);

			res.country_code = data.country_code;
			(location_pool.seek(data.state_offset), location_pool.str_nt(res.state));
			(location_pool.seek(data.city_offset), location_pool.str_nt(res.city));
		}

		return res;
	}
}
