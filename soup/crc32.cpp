#include "crc32.hpp"

#include <array>

namespace soup
{
	static std::array<uint32_t, 256> generate_crc_lookup_table() noexcept
	{
		constexpr uint32_t reversed_polynomial = 0xEDB88320ul;

		auto table = std::array<uint32_t, 256>{};
		for(uint32_t i = 0; i != table.size(); ++i)
		{
			uint32_t checksum = i;
			for (auto j = 0; j != 8; ++j)
			{
				checksum = (checksum >> 1) ^ ((checksum & 0x1u) * reversed_polynomial);
			}
			table[i] = checksum;
		}
		return table;
	}

	uint32_t crc32::hash(const std::string& data)
	{
		static auto const table = generate_crc_lookup_table();

		uint32_t checksum = 0xFFFFFFFFul;
		for (const auto& c : data)
		{
			const uint32_t lookupIndex = (checksum ^ (uint8_t)c) & 0xff;
			checksum = (checksum >> 8) ^ table[lookupIndex];
		}
		checksum ^= 0xFFFFFFFFu;
		return checksum;
	}
}
