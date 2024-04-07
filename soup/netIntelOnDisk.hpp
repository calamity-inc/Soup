#pragma once

#include "base.hpp"
#if SOUP_CPP20

#include <filesystem>

#include "DiskDbReader.hpp"
#include "FileReader.hpp"
#include "netIntelLocationData.hpp"

NAMESPACE_SOUP
{
	struct netIntelOnDisk
	{
		FileReader location_pool;
		FileReader ipv4tolocation_fr;
		DiskDbReader ipv4tolocation_dr;

		netIntelOnDisk(const std::filesystem::path& dir);

		[[nodiscard]] netIntelLocationDataSelfContained getLocationByIpv4(native_u32_t ip);
	};
}

#endif
